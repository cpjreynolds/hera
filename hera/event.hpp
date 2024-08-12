// hera
// Copyright (C) 2024  Cole Reynolds
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef HERA_EVENT_HPP
#define HERA_EVENT_HPP

#include <hera/common.hpp>
#include <hera/log.hpp>

namespace hera {

class Observer;

template<typename... Ts>
struct thunk {
private:
    using fn_type = void (*)(void*, Ts&&...);

    template<auto mem_ptr, typename T>
        requires std::is_pointer_v<T>
    static consteval fn_type make_thunk()
    {
        if constexpr (std::derived_from<std::remove_pointer_t<T>, Observer>) {
            return [](void* iptr, Ts&&... args) {
                T i = static_cast<T>(reinterpret_cast<Observer*>(iptr));
                (i->*mem_ptr)(std::forward<Ts>(args)...);
            };
        }
        else {
            return [](void* iptr, Ts&&... args) {
                (reinterpret_cast<T>(iptr)->*mem_ptr)(
                    std::forward<Ts>(args)...);
            };
        }
    }

    template<typename C>
        requires std::is_pointer_v<C>
    static consteval fn_type make_thunk()
    {
        if constexpr (std::derived_from<std::remove_pointer_t<C>, Observer>) {
            return [](void* iptr, Ts&&... args) {
                C i = static_cast<C>(reinterpret_cast<Observer*>(iptr));
                i->operator()(std::forward<Ts>(args)...);
            };
        }
        else {
            return [](void* iptr, Ts&&... args) {
                reinterpret_cast<C>(iptr)->operator()(
                    std::forward<Ts>(args)...);
            };
        }
    }

    template<typename T>
    static constexpr void* make_ptr(T* ptr)
    {
        if constexpr (std::derived_from<T, Observer>) {
            return static_cast<T::Observer*>(ptr);
        }
        else {
            return ptr;
        }
    }

    template<typename T>
    static constexpr void* make_ptr(const T* ptr)
    {
        if constexpr (std::derived_from<T, Observer>) {
            return const_cast<void*>(static_cast<const T::Observer*>(ptr));
        }
        else {
            return const_cast<void*>(ptr);
        }
    }

    static_assert(sizeof(uint128_t) == (sizeof(fn_type) + sizeof(void*)));

public:
    union {
        uint128_t key;
        struct {
#if defined(HERA_LITTLE_ENDIAN)
            fn_type thunktion;
            void* instance;
#elif defined(HERA_BIG_ENDIAN)
            void* instance;
            fn_type thunktion;
#endif
        };
    };

    constexpr thunk(fn_type thunktion, void* instance)
#if defined(HERA_LITTLE_ENDIAN)
        : thunktion{thunktion},
          instance{instance} {};
#elif defined(HERA_BIG_ENDIAN)
        : instance{instance},
          thunktion{thunktion} {};
#endif

    template<auto fun_ptr>
        requires invocable<decltype(fun_ptr), Ts...>
    static constexpr thunk bind()
    {
        return {
            [](void*, Ts&&... args) { (*fun_ptr)(std::forward<Ts>(args)...); },
            nullptr};
    }

    template<auto mem_ptr, typename T>
        requires invocable<decltype(mem_ptr), T*, Ts...>
    static constexpr thunk bind(T* obj)
    {
        return {make_thunk<mem_ptr, decltype(obj)>(), make_ptr(obj)};
    }

    template<auto mem_ptr, typename T>
        requires invocable<decltype(mem_ptr), const T*, Ts...>
    static constexpr thunk bind(const T* obj)
    {
        return {make_thunk<mem_ptr, decltype(obj)>(), make_ptr(obj)};
    }

    template<typename C>
        requires invocable<C, Ts...>
    static constexpr thunk bind(C* obj)
    {
        return {make_thunk<decltype(obj)>(), make_ptr(obj)};
    }

    template<typename C>
        requires invocable<const C, Ts...>
    static constexpr thunk bind(const C* obj)
    {
        return {make_thunk<decltype(obj)>(), make_ptr(obj)};
    }

    template<typename... Args>
    void operator()(Args&&... args) const
    {
        thunktion(instance, static_cast<Ts&&>(args)...);
    }

    constexpr friend bool operator==(const thunk& lhs, const thunk& rhs)
    {
        return lhs.key == rhs.key;
    }
    constexpr friend auto operator<=>(const thunk& lhs, const thunk& rhs)
    {
        return lhs.key <=> rhs.key;
    }

    constexpr friend bool operator==(const thunk& lhs, const uintptr_t& rhs)
    {
        return lhs.instance == rhs;
    }
    constexpr friend auto operator<=>(const thunk& lhs, const uintptr_t& rhs)
    {
        return lhs.instance <=> rhs;
    }

    constexpr friend bool operator==(const thunk& lhs, const void* rhs)
    {
        return lhs.instance == rhs;
    }
    constexpr friend auto operator<=>(const thunk& lhs, const void* rhs)
    {
        return lhs.instance <=> rhs;
    }
};

namespace detail {
struct signal_block_base {
    virtual void move_to(const void* from, void* to) = 0;
    virtual void copy_to(const void* from, void* to) = 0;
    virtual void do_disconnect(const void*) = 0;

protected:
    ~signal_block_base() {}
};
}; // namespace detail

class Observer {
    template<typename... Ts>
    friend class signal;

    using block_base = detail::signal_block_base;

    struct connection {
        void* iptr;
        weak_ptr<block_base> sigblock;
    };

    static constexpr auto conn_iptr = [](const connection& v) -> void* const& {
        return v.iptr;
    };

    // mutable vector<weak_ptr<detail::signal_block_base>> signals;
    mutable vector<connection> signals;

protected:
    Observer() {};

    ~Observer()
    {
        for (auto& [iptr, wptr] : signals) {
            if (auto ptr = wptr.lock()) {
                ptr->do_disconnect((const void*)iptr);
            }
        }
    }

    auto equal_range(const void* value) const
    {
        return ranges::equal_range(signals, value, {}, conn_iptr);
    }

    auto equal_this(this const auto& self) { return self.equal_range(&self); }

    template<typename Self>
    void copy_from(this Self& self, const Self& other)
    {
        auto to_copy = other.equal_this();

        auto repointed =
            views::transform(to_copy, [sptr = &self](const connection& v) {
                return connection{sptr, v.sigblock};
            });

        auto pos = ranges::equal_range(self.signals, &self, {}, conn_iptr);
    }

    /*
    ~Observer()
    {
        for (auto& wptr : signals) {
            if (auto ptr = wptr.lock()) {
                ptr->do_disconnect(this);
            }
        }
    }

    Observer(const Observer& other) : signals{other.signals}
    {
        for (auto& wptr : signals) {
            if (auto ptr = wptr.lock()) {
                ptr->copy_to(&other, this);
            }
        }
    }
    Observer& operator=(const Observer& other)
    {
        Observer(other).swap(*this);
        return *this;
    }

    Observer(Observer&& other) : signals{std::move(other.signals)}
    {
        for (auto& wptr : signals) {
            if (auto ptr = wptr.lock()) {
                ptr->move_to(&other, this);
            }
        }
    }
    Observer& operator=(Observer&& other)
    {
        Observer(std::move(other)).swap(*this);
        return *this;
    }
    */

    void swap(Observer& other) { signals.swap(other.signals); }
};

struct tester : Observer {
    using Observer::copy_from;
};

void test()
{
    tester x;
    tester y;

    x.copy_from(y);
}

template<typename... Ts>
class signal {
public:
    // immediate notification of all slots.
    template<typename... Args>
    void operator()(Args&&... args) const
    {
        for (auto&& fn : block->slots) {
            fn(std::forward<Args>(args)...);
        }
    }

    // post an event to the internal queue.
    template<typename... Args>
    void post(Args&&... args)
    {
        block->evqueue.emplace_back(static_cast<Ts&&>(args)...);
    }

    // flush the event queue to all slots.
    void flush()
    {
        if (block->evqueue.empty()) {
            return;
        }
        for (auto&& fn : block->slots) {
            for (auto&& ev : block->evqueue) {
                std::apply(fn, ev);
            }
        }
        block->evqueue.clear();
    }

    // connects an object and member function
    template<auto mem_ptr, typename T>
        requires invocable<decltype(mem_ptr), T*, Ts...>
    void connect(T* obj)
    {
        block->do_connect(thunk<Ts...>::template bind<mem_ptr>(obj));

        if constexpr (std::derived_from<T, Observer>) {
            static_cast<Observer*>(obj)->signals.emplace_back(block);
        }
    }

    template<auto mem_ptr, typename T>
    void connect(T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }

    // connects a const object and member function
    template<auto mem_ptr, typename T>
        requires invocable<decltype(mem_ptr), const T*, Ts...>
    void connect(const T* obj)
    {
        block->do_connect(thunk<Ts...>::template bind<mem_ptr>(obj));

        if constexpr (std::derived_from<T, Observer>) {
            static_cast<const Observer*>(obj)->signals.emplace_back(block);
        }
    }

    template<auto mem_ptr, typename T>
    void connect(const T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }

    template<typename T, void (T::*mem_ptr)(Ts...)>
    void connect(T* obj)
    {
        connect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) noexcept>
    void connect(T* obj)
    {
        connect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) &>
    void connect(T* obj)
    {
        connect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) & noexcept>
    void connect(T* obj)
    {
        connect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const>
    void connect(const T* obj)
    {
        connect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const noexcept>
    void connect(const T* obj)
    {
        connect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const&>
    void connect(const T* obj)
    {
        connect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const& noexcept>
    void connect(const T* obj)
    {
        connect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...)>
    void connect(T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) noexcept>
    void connect(T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) &>
    void connect(T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) & noexcept>
    void connect(T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const>
    void connect(const T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const noexcept>
    void connect(const T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const&>
    void connect(const T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const& noexcept>
    void connect(const T& obj)
    {
        connect<mem_ptr>(std::addressof(obj));
    }

    // connects a function pointer
    template<void (*fun_ptr)(Ts...)>
    void connect()
    {
        block->do_connect(thunk<Ts...>::template bind<fun_ptr>());
    }
    // connects a function pointer
    template<void (*fun_ptr)(Ts...) noexcept>
    void connect()
    {
        block->do_connect(thunk<Ts...>::template bind<fun_ptr>());
    }

    // connects a callable object.
    template<typename C>
        requires invocable<C, Ts...>
    void connect(C* callable)
    {
        block->do_connect(thunk<Ts...>::template bind<C>(callable));
        if constexpr (std::derived_from<C, Observer>) {
            static_cast<Observer*>(callable)->signals.emplace_back(block);
        }
    }
    // connects a const callable object.
    template<typename C>
        requires invocable<const C, Ts...>
    void connect(const C* callable)
    {
        block->do_connect(thunk<Ts...>::template bind<C>(callable));
        if constexpr (std::derived_from<C, Observer>) {
            static_cast<const Observer*>(callable)->signals.emplace_back(block);
        }
    }

    // connects a callable object.
    template<typename C>
    void connect(C& callable)
    {
        connect(std::addressof(callable));
    }
    // connects a const callable object.
    template<typename C>
    void connect(const C& callable)
    {
        connect(std::addressof(callable));
    }

    // member function disconnect
    template<auto mem_ptr, typename T>
        requires invocable<decltype(mem_ptr), T*, Ts...>
    void disconnect(const T* obj)
    {
        block->do_disconnect(thunk<Ts...>::template bind<mem_ptr>(obj));
    }

    template<typename T, void (T::*mem_ptr)(Ts...)>
    void disconnect(T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) noexcept>
    void disconnect(T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) &>
    void disconnect(T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) & noexcept>
    void disconnect(T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const>
    void disconnect(const T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const noexcept>
    void disconnect(const T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const&>
    void disconnect(const T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const& noexcept>
    void disconnect(const T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    template<typename T, void (T::*mem_ptr)(Ts...)>
    void disconnect(T& obj)
    {
        disconnect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) noexcept>
    void disconnect(T& obj)
    {
        disconnect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) &>
    void disconnect(T& obj)
    {
        disconnect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) & noexcept>
    void disconnect(T& obj)
    {
        disconnect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const>
    void disconnect(const T& obj)
    {
        disconnect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const noexcept>
    void disconnect(const T& obj)
    {
        disconnect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const&>
    void disconnect(const T& obj)
    {
        disconnect<mem_ptr>(std::addressof(obj));
    }
    template<typename T, void (T::*mem_ptr)(Ts...) const& noexcept>
    void disconnect(const T& obj)
    {
        disconnect<mem_ptr>(std::addressof(obj));
    }

    // function pointer disconnect.
    template<void (*fun_ptr)(Ts...)>
    void disconnect()
    {
        block->do_disconnect(thunk<Ts...>::template bind<fun_ptr>());
    }

    // function pointer disconnect.
    template<void (*fun_ptr)(Ts...) noexcept>
    void disconnect()
    {
        block->do_disconnect(thunk<Ts...>::template bind<fun_ptr>());
    }

    // disconnects a callable or all members of an instance
    template<typename T>
    void disconnect(const T* obj)
    {
        block->do_disconnect(obj);
    }

    // disconnects a callable or all members of an instance
    template<typename T>
    void disconnect(const T& obj)
    {
        block->do_disconnect(std::addressof(obj));
    }

    // disconnects all slots
    void disconnect_all() { block->slots.clear(); }

private:
    struct signal_block final : detail::signal_block_base {
        vector<thunk<Ts...>> slots;
        vector<tuple<Ts...>> evqueue;

        void do_connect(const thunk<Ts...>& tk)
        {
            slots.emplace(ranges::upper_bound(slots, tk), tk);
        }

        void do_disconnect(const void* ptr) override
        {
            auto [b, e] = ranges::equal_range(slots, ptr, std::less{});
            slots.erase(b, e);
        }

        void do_disconnect(const thunk<Ts...>& tk)
        {
            auto [b, e] = ranges::equal_range(slots, tk);
            slots.erase(b, e);
        }

        void move_to(const void* from, void* to) override
        {
            auto iter = ranges::equal_range(slots, from, std::less{});
            for (auto& slot : iter) {
                slot.instance = to;
            }
        }

        void copy_to(const void* from, void* to) override
        {
            auto iter = ranges::equal_range(slots, from, std::less{});
            for (auto slot : iter) {
                slot.instance = to;
                do_connect(slot);
            }
        }
    };

    shared_ptr<signal_block> block = std::make_shared<signal_block>();
};

} // namespace hera

#endif
