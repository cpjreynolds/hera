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
#include <hera/utility.hpp>

namespace hera {

class Observer;

template<typename>
struct thunk;

template<typename R, typename... Ts>
struct thunk<R(Ts...)> {
private:
    using fn_type = R (*)(void*, Ts&&...);

    template<auto mem_ptr, typename T>
        requires std::is_pointer_v<T>
    static consteval fn_type make_thunk()
    {
        return [](void* iptr, Ts&&... args) {
            return (reinterpret_cast<T>(iptr)->*mem_ptr)(
                std::forward<Ts>(args)...);
        };
    }

    template<typename C>
        requires std::is_pointer_v<C>
    static consteval fn_type make_thunk()
    {
        return [](void* iptr, Ts&&... args) {
            return reinterpret_cast<C>(iptr)->operator()(
                std::forward<Ts>(args)...);
        };
    }

    static_assert(sizeof(uint128_t) == (sizeof(fn_type) + sizeof(void*)));

public:
#if defined(HERA_LITTLE_ENDIAN)
    union {
        uint128_t key;
        struct {
            fn_type thunktion;
            void* instance;
        };
    };
    constexpr thunk(fn_type thunktion, void* instance) noexcept
        : thunktion{thunktion},
          instance{instance} {};
#elif defined(HERA_BIG_ENDIAN)
    union {
        uint128_t key;
        struct {
            void* instance;
            fn_type thunktion;
        };
    };
    constexpr thunk(fn_type thunktion, void* instance) noexcept
        : instance{instance},
          thunktion{thunktion} {};
#endif

    template<auto fun_ptr>
        requires invocable_r<R, decltype(fun_ptr), Ts...>
    static constexpr thunk bind()
    {
        return {[](void*, Ts&&... args) {
                    return (*fun_ptr)(std::forward<Ts>(args)...);
                },
                nullptr};
    }

    template<auto mem_ptr, typename T>
        requires invocable_r<R, decltype(mem_ptr), T*, Ts...>
    static constexpr thunk bind(T* obj)
    {
        return {make_thunk<mem_ptr, decltype(obj)>(), obj};
    }

    template<auto mem_ptr, typename T>
        requires invocable_r<R, decltype(mem_ptr), const T*, Ts...>
    static constexpr thunk bind(const T* obj)
    {
        return {make_thunk<mem_ptr, decltype(obj)>(), (void*)obj};
    }

    template<typename C>
        requires invocable_r<R, C, Ts...>
    static constexpr thunk bind(C* obj)
    {
        return {make_thunk<decltype(obj)>(), obj};
    }

    template<typename C>
        requires invocable_r<R, const C, Ts...>
    static constexpr thunk bind(const C* obj)
    {
        return {make_thunk<decltype(obj)>(), (void*)obj};
    }

    template<typename... Args>
    R operator()(Args&&... args) const
    {
        return thunktion(instance, static_cast<Ts&&>(args)...);
    }

    constexpr friend bool operator==(const thunk& lhs, const thunk& rhs)
    {
        return lhs.key == rhs.key;
    }
    constexpr friend auto operator<=>(const thunk& lhs, const thunk& rhs)
    {
        return lhs.key <=> rhs.key;
    }
};

class signal_block_base {
public:
    virtual void move_to(const void* from, void* to) = 0;
    virtual void copy_to(const void* from, void* to) = 0;
    virtual void do_disconnect(const void*) = 0;

protected:
    ~signal_block_base() {}
};

class Observer {
    template<typename>
    friend class signal;

    using block_base = signal_block_base;

    struct connection {
        void* iptr;
        weak_ptr<block_base> sigblock;
    };

    static constexpr auto conn_iptr = [](const connection& v) -> void* const& {
        return v.iptr;
    };

    // mutable vector<weak_ptr<detail::signal_block_base>> signals;
    mutable vector<connection> signals;

    template<typename Self>
        requires(!same_as<Self, Observer>)
    void add_connection(this Self& self, weak_ptr<block_base> bptr)
    {
        auto pos =
            ranges::upper_bound(self.signals, (void*)&self, {}, conn_iptr);
        self.signals.emplace(pos, &self, std::move(bptr));
    }

    auto equal_range(const void* value) const
    {
        return ranges::equal_range(signals, value, {}, conn_iptr);
    }

    auto equal_this(this const auto& self) { return self.equal_range(&self); }

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

    template<typename Self>
    void copy_from(this Self& self, const Self& other)
    {
        auto to_copy = other.equal_this() | ranges::to<vector>();

        for (auto& [iptr, wptr] : to_copy) {
            iptr = &self;
            if (auto ptr = wptr.lock()) {
                ptr->copy_to(&other, &self);
            }
        }

        auto pos =
            ranges::upper_bound(self.signals, (void*)&self, {}, conn_iptr);
        self.signals.insert_range(pos, to_copy);
    }

    Observer(Observer&& other) : signals{std::move(other.signals)}
    {
        for (auto& [iptr, wptr] : signals) {
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

    void swap(Observer& other) { signals.swap(other.signals); }
};

struct tester : Observer {
    using Observer::copy_from;
};

inline void test()
{
    tester x;
    tester y;

    x.copy_from(y);
}

template<typename>
class basic_signal_block;

template<typename R, typename... Ts>
class basic_signal_block<R(Ts...)> : signal_block_base {
protected:
    using thunk_type = thunk<R(Ts...)>;

    mutable shared_mutex mtx;
    vector<thunk<R(Ts...)>> slots;

    static constexpr auto proj = [](const thunk_type& t) -> void* const& {
        return t.instance;
    };

public:
    template<typename... Args>
    void fire(Args&&... args) const
    {
        shared_lock lk{mtx};
        for (auto& fn : slots) {
            fn(std::forward<Args>(args)...);
        }
    }

    template<typename Accumulate, typename... Args>
    void fire_accumulate(Accumulate&& acc, Args&&... args) const
    {
        shared_lock lk{mtx};
        for (auto& fn : slots) {
            acc(fn(std::forward<Args>(args)...));
        }
    }

    void do_connect(const thunk_type& tk)
    {
        scoped_lock lk{mtx};
        slots.emplace(ranges::upper_bound(slots, tk), tk);
    }

    void do_disconnect(const void* ptr) final override
    {
        scoped_lock lk{mtx};
        auto [b, e] = ranges::equal_range(slots, ptr, {}, proj);
        slots.erase(b, e);
    }

    void do_disconnect(const thunk_type& tk)
    {
        scoped_lock lk{mtx};
        auto [b, e] = ranges::equal_range(slots, tk);
        slots.erase(b, e);
    }

    void move_to(const void* from, void* to) final override
    {
        scoped_lock lk{mtx};
        auto [b, e] = ranges::equal_range(slots, from, {}, proj);

        vector<thunk_type> to_insert{b, e};
        for (auto& slot : to_insert) {
            slot.instance = to;
        }
        slots.erase(b, e);
        auto pos = ranges::upper_bound(slots, to_insert.front());
        slots.insert_range(pos, to_insert);
    }

    void copy_to(const void* from, void* to) final override
    {
        scoped_lock lk{mtx};
        auto to_insert =
            ranges::equal_range(slots, from, {}, proj) | ranges::to<vector>();

        for (auto& slot : to_insert) {
            slot.instance = to;
        }
        auto pos = ranges::upper_bound(slots, to_insert.front());
        slots.insert_range(pos, to_insert);
    }
};

template<typename>
struct queue_signal_block;

template<typename R, typename... Ts>
    requires std::is_default_constructible_v<tuple<Ts...>>
struct queue_signal_block<R(Ts...)> : basic_signal_block<R(Ts...)> {
    concurrent_queue<tuple<Ts...>> evqueue;

    template<typename... Args>
    void do_post(Args&&... args)
    {
        evqueue.emplace(static_cast<Ts&&>(args)...);
    }

    void flush()
    {
        shared_lock lk{this->mtx};

        tuple<Ts...> event;

        while (evqueue.try_pop(event)) {
            for (auto& fn : this->slots) {
                std::apply(fn, event);
            }
        }
    }

    template<typename Accumulate>
    void flush_accumulate(Accumulate&& acc)
    {
        shared_lock lk{this->mtx};

        tuple<Ts...> event;

        while (evqueue.try_pop(event)) {
            for (auto& fn : this->slots) {
                acc(std::apply(fn, event));
            }
        }
    }
};

template<typename>
class signal;

template<typename R, typename... Ts>
class signal<R(Ts...)> {
    using thunk_type = thunk<R(Ts...)>;

public:
    // immediate notification of all slots.
    template<typename... Args>
    void operator()(Args&&... args) const
    {
        block->fire(std::forward<Args>(args)...);
    }

    // post an event to the internal queue.
    template<typename... Args>
    void post(Args&&... args)
    {
        block->evqueue.emplace(static_cast<Ts&&>(args)...);
    }

    // flush the event queue to all slots.
    void flush()
    {
        shared_lock lk{block->slot_mtx};

        tuple<Ts...> event;

        while (block->evqueue.try_pop(event)) {
            for (auto& fn : block->slots) {
                std::apply(fn, event);
            }
        }
    }

    // connects an object and member function
    template<auto mem_ptr, typename T>
        requires invocable_r<R, decltype(mem_ptr), T*, Ts...>
    void connect(T* obj)
    {
        block->do_connect(thunk_type::template bind<mem_ptr>(obj));

        if constexpr (std::derived_from<T, Observer>) {
            obj->add_connection(block);
        }
    }

    // connects a const object and member function
    template<auto mem_ptr, typename T>
        requires invocable_r<R, decltype(mem_ptr), const T*, Ts...>
    void connect(const T* obj)
    {
        block->do_connect(thunk_type::template bind<mem_ptr>(obj));

        if constexpr (std::derived_from<T, Observer>) {
            obj->add_connection(block);
        }
    }

    // connects an object and member function
    template<typename T, R (T::*mem_ptr)(Ts...)>
    void connect(T* obj)
    {
        connect<mem_ptr>(obj);
    }
    // connects an object and member function
    template<typename T, R (T::*mem_ptr)(Ts...) noexcept>
    void connect(T* obj)
    {
        connect<mem_ptr>(obj);
    }
    // connects a const object and member function
    template<typename T, R (T::*mem_ptr)(Ts...) const>
    void connect(const T* obj)
    {
        connect<mem_ptr>(obj);
    }
    // connects a const object and member function
    template<typename T, R (T::*mem_ptr)(Ts...) const noexcept>
    void connect(const T* obj)
    {
        connect<mem_ptr>(obj);
    }

    // connects a function pointer
    template<R (*fun_ptr)(Ts...)>
    void connect()
    {
        block->do_connect(thunk_type::template bind<fun_ptr>());
    }
    // connects a function pointer
    template<R (*fun_ptr)(Ts...) noexcept>
    void connect()
    {
        block->do_connect(thunk_type::template bind<fun_ptr>());
    }

    // connects a callable object.
    template<typename C>
        requires invocable_r<R, C, Ts...>
    void connect(C* callable)
    {
        block->do_connect(thunk_type::template bind<C>(callable));

        if constexpr (std::derived_from<C, Observer>) {
            callable->add_connection(block);
        }
    }
    // connects a const callable object.
    template<typename C>
        requires invocable_r<R, const C, Ts...>
    void connect(const C* callable)
    {
        block->do_connect(thunk_type::template bind<C>(callable));

        if constexpr (std::derived_from<C, Observer>) {
            callable->add_connection(block);
        }
    }

    // member function disconnect
    template<auto mem_ptr, typename T>
        requires invocable_r<R, decltype(mem_ptr), T*, Ts...>
    void disconnect(const T* obj)
    {
        block->do_disconnect(thunk_type::template bind<mem_ptr>(obj));
    }
    // member function disconnect
    template<typename T, R (T::*mem_ptr)(Ts...)>
    void disconnect(const T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    // member function disconnect
    template<typename T, R (T::*mem_ptr)(Ts...) noexcept>
    void disconnect(const T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    // member function disconnect
    template<typename T, R (T::*mem_ptr)(Ts...) const>
    void disconnect(const T* obj)
    {
        disconnect<mem_ptr>(obj);
    }
    // member function disconnect
    template<typename T, R (T::*mem_ptr)(Ts...) const noexcept>
    void disconnect(const T* obj)
    {
        disconnect<mem_ptr>(obj);
    }

    // function pointer disconnect.
    template<R (*fun_ptr)(Ts...)>
    void disconnect()
    {
        block->do_disconnect(thunk_type::template bind<fun_ptr>());
    }

    // function pointer disconnect.
    template<R (*fun_ptr)(Ts...) noexcept>
    void disconnect()
    {
        block->do_disconnect(thunk_type::template bind<fun_ptr>());
    }

    // disconnects a callable or all members of an instance
    template<typename T>
    void disconnect(const T* obj)
    {
        block->do_disconnect(obj);
    }

    // disconnects all slots
    void disconnect_all() { block->slots.clear(); }

private:
    struct signal_block final : signal_block_base {
        mutable shared_mutex slot_mtx;
        vector<thunk_type> slots;
        concurrent_queue<tuple<Ts...>> evqueue;

        static constexpr auto proj = [](const thunk_type& t) -> void* const& {
            return t.instance;
        };

        template<typename... Args>
        void fire(Args&&... args) const
        {
            shared_lock lk{slot_mtx};
            for (auto& fn : slots) {
                fn(std::forward<Args>(args)...);
            }
        }

        void do_connect(const thunk_type& tk)
        {
            slots.emplace(ranges::upper_bound(slots, tk), tk);
        }

        void do_disconnect(const void* ptr) override
        {
            auto [b, e] = ranges::equal_range(slots, ptr, {}, proj);
            slots.erase(b, e);
        }

        void do_disconnect(const thunk_type& tk)
        {
            auto [b, e] = ranges::equal_range(slots, tk);
            slots.erase(b, e);
        }

        void move_to(const void* from, void* to) override
        {
            auto [b, e] = ranges::equal_range(slots, from, {}, proj);

            vector<thunk_type> to_insert{b, e};
            for (auto& slot : to_insert) {
                slot.instance = to;
            }
            slots.erase(b, e);
            auto pos = ranges::upper_bound(slots, to_insert.front());
            slots.insert_range(pos, to_insert);
        }

        void copy_to(const void* from, void* to) override
        {
            auto to_insert = ranges::equal_range(slots, from, {}, proj) |
                             ranges::to<vector>();

            for (auto& slot : to_insert) {
                slot.instance = to;
            }
            auto pos = ranges::upper_bound(slots, to_insert.front());
            slots.insert_range(pos, to_insert);
        }
    };

    shared_ptr<signal_block> block = std::make_shared<signal_block>();
};

} // namespace hera

#endif
