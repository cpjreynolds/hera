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

using thunk_key = array<uintptr_t, 2>;

template<typename... Ts>
struct thunk {
    using fn_type = void (*)(void*, Ts&&...);

    void* instance;
    fn_type thunktion;

    static constexpr thunk bind(const thunk_key& key)
    {
        return {reinterpret_cast<void*>(key[0]),
                reinterpret_cast<fn_type>(key[1])};
    }

    constexpr operator thunk_key() const
    {
        return {reinterpret_cast<uintptr_t>(instance),
                reinterpret_cast<uintptr_t>(thunktion)};
    }

    template<auto fun_ptr>
        requires invocable<decltype(fun_ptr), Ts...>
    static constexpr thunk bind()
    {
        return {nullptr, [](void*, Ts&&... args) {
                    (*fun_ptr)(std::forward<Ts>(args)...);
                }};
    }

    template<auto mem_ptr, typename T>
        requires invocable<decltype(mem_ptr), T*, Ts...>
    static constexpr thunk bind(T* obj)
    {
        return {obj, [](void* iptr, Ts&&... args) {
                    (static_cast<T*>(iptr)->*mem_ptr)(
                        std::forward<Ts>(args)...);
                }};
    }

    template<auto mem_ptr, typename T>
        requires invocable<decltype(mem_ptr), const T*, Ts...>
    static constexpr thunk bind(const T* obj)
    {
        return {(void*)obj, [](void* iptr, Ts&&... args) {
                    (static_cast<const T*>(iptr)->*mem_ptr)(
                        std::forward<Ts>(args)...);
                }};
    }

    template<typename C>
        requires invocable<C, Ts...>
    static constexpr thunk bind(C* callable)
    {
        return {callable, [](void* iptr, Ts&&... args) {
                    static_cast<C*>(iptr)->operator()(
                        std::forward<Ts>(args)...);
                }};
    }

    template<typename C>
        requires invocable<const C, Ts...>
    static constexpr thunk bind(const C* callable)
    {
        return {(void*)callable, [](void* iptr, Ts&&... args) {
                    static_cast<const C*>(iptr)->operator()(
                        std::forward<Ts>(args)...);
                }};
    }

    template<typename... Args>
    void operator()(Args&&... args) const
    {
        thunktion(instance, static_cast<Ts&&>(args)...);
    }

    constexpr friend bool operator==(const thunk& lhs, const thunk& rhs)
    {
        return static_cast<thunk_key>(lhs) == static_cast<thunk_key>(rhs);
    }
    constexpr friend auto operator<=>(const thunk& lhs, const thunk& rhs)
    {
        return static_cast<thunk_key>(lhs) <=> static_cast<thunk_key>(rhs);
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
    virtual void do_disconnect(const void*) = 0;
    // virtual ~signal_block_base() {}
protected:
    ~signal_block_base() {}
};
}; // namespace detail

class observer {
    template<typename... Ts>
    friend class signal;

    mutable vector<pair<weak_ptr<detail::signal_block_base>, const void*>>
        deleters;

protected:
    observer() {};
    ~observer()
    {
        for (auto&& [wptr, obj] : deleters) {
            if (auto ptr = wptr.lock()) {
                ptr->do_disconnect(obj);
            }
        }
    }
};

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
        block->evqueue.emplace_back(std::forward<Args>(args)...);
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

        if constexpr (std::derived_from<T, observer>) {
            static_cast<observer*>(obj)->deleters.emplace_back(block, obj);
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

        if constexpr (std::derived_from<T, observer>) {
            static_cast<const observer*>(obj)->deleters.emplace_back(block,
                                                                     obj);
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
        if constexpr (std::derived_from<C, observer>) {
            static_cast<observer*>(callable)->deleters.emplace_back(block,
                                                                    callable);
        }
    }
    // connects a const callable object.
    template<typename C>
        requires invocable<const C, Ts...>
    void connect(const C* callable)
    {
        block->do_connect(thunk<Ts...>::template bind<C>(callable));
        if constexpr (std::derived_from<C, observer>) {
            static_cast<const observer*>(callable)->deleters.emplace_back(
                block, callable);
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
    };

    shared_ptr<signal_block> block = std::make_shared<signal_block>();
};

} // namespace hera

#endif
