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

template<typename... Ts>
class thunk {
    using fn_type = void (*)(void*, uint128_t, Ts...);
    uint128_t mptr = 0;
    void* ptr;
    fn_type thunktion;

public:
    thunk(const thunk&) = default;
    thunk& operator=(const thunk&) = default;
    thunk(thunk&&) = default;
    thunk& operator=(thunk&&) = default;

    void operator()(Ts... args)
    {
        thunktion(ptr, mptr, std::forward<Ts>(args)...);
    }

    template<typename T, typename M>
        requires std::invocable<M T::*, T*, Ts...>
    thunk(T* instance, M T::*mem_ptr)
        : mptr{bit_cast<uint128_t>(mem_ptr)},
          ptr{instance},
          thunktion{[](void* iptr, uint128_t member, Ts... args) {
              auto i = static_cast<T*>(iptr);
              auto m = bit_cast<M T::*>(member);
              std::invoke(m, i, std::forward<Ts>(args)...);
          }} {};

    template<typename T, typename M>
        requires std::invocable<M T::*, const T*, Ts...>
    thunk(const T* instance, M T::*mem_ptr)
        : mptr{bit_cast<uint128_t>(mem_ptr)},
          ptr{(void*)instance},
          thunktion{[](void* iptr, uint128_t member, Ts... args) {
              const auto i = static_cast<const T*>(iptr);
              auto m = bit_cast<M T::*>(member);
              std::invoke(m, i, std::forward<Ts>(args)...);
          }} {};

    template<typename T, typename M>
        requires std::invocable<M T::*, T*, Ts...>
    thunk(T& instance, M T::*mem_ptr)
        : thunk{std::addressof(instance), mem_ptr} {};

    template<typename T, typename M>
        requires std::invocable<M T::*, const T*, Ts...>
    thunk(const T& instance, M T::*mem_ptr)
        : thunk{std::addressof(instance), mem_ptr} {};

    thunk(void (*fun_ptr)(Ts...))
        : ptr{fun_ptr},
          thunktion{[](void* fptr, uint128_t, Ts... args) {
              static_cast<decltype(fun_ptr)>(fptr)(std::forward<Ts>(args)...);
          }} {};

    thunk(void (*fun_ptr)(Ts...) noexcept)
        : ptr{fun_ptr},
          thunktion{[](void* fptr, uint128_t, Ts... args) {
              static_cast<decltype(fun_ptr)>(fptr)(std::forward<Ts>(args)...);
          }} {};

    template<typename C>
        requires std::invocable<C, Ts...>
    thunk(C* instance)
        : ptr{instance},
          thunktion{[](void* iptr, uint128_t, Ts... args) {
              static_cast<C*>(iptr)->operator()(std::forward<Ts>(args)...);
          }} {};

    template<typename C>
        requires std::invocable<const C, Ts...>
    thunk(const C* instance)
        : ptr{(void*)instance},
          thunktion{[](void* iptr, uint128_t, Ts... args) {
              static_cast<const C*>(iptr)->operator()(
                  std::forward<Ts>(args)...);
          }} {};

    template<typename C>
        requires std::invocable<C, Ts...>
    thunk(C& instance) : thunk{std::addressof(instance)} {};

    template<typename C>
        requires std::invocable<C, Ts...>
    thunk(const C& instance) : thunk{std::addressof(instance)} {};

    friend bool operator==(const thunk& lhs, const uintptr_t& rhs)
    {
        lhs.ptr == rhs;
    }
    friend auto operator<=>(const thunk& lhs, const uintptr_t& rhs)
    {
        lhs.ptr <=> rhs;
    }

    friend bool operator==(const thunk& lhs,
                           const pair<uintptr_t, uint128_t>& rhs)
    {
        return pair{lhs.ptr, lhs.mptr} == rhs;
    }
    friend bool operator<=>(const thunk& lhs,
                            const pair<uintptr_t, uint128_t>& rhs)
    {
        return pair{lhs.ptr, lhs.mptr} <=> rhs;
    }

    friend bool operator==(const thunk& lhs, const thunk& rhs)
    {
        return pair{lhs.ptr, lhs.mptr} == pair{rhs.ptr, rhs.mptr};
    }
    friend auto operator<=>(const thunk& lhs, const thunk& rhs)
    {
        return pair{lhs.ptr, lhs.mptr} <=> pair{rhs.ptr, rhs.mptr};
    }

    friend bool operator==(const thunk& lhs, const void* rhs_p)
    {
        return lhs.ptr == rhs_p;
    }
    friend auto operator<=>(const thunk& lhs, const void* rhs_p)
    {
        return lhs.ptr <=> rhs_p;
    }

    friend bool operator==(const thunk& lhs, const pair<void*, uint128_t>& rhs)
    {
        return pair{lhs.ptr, lhs.mptr} == rhs;
    }
    friend auto operator<=>(const thunk& lhs, const pair<void*, uint128_t>& rhs)
    {
        return pair{lhs.ptr, lhs.mptr} <=> rhs;
    }
};

template<typename T, typename... Ts>
thunk(T*, void (T::*)(Ts...)) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(T*, void (T::*)(Ts...) noexcept) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(T*, void (T::*)(Ts...) &) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(T*, void (T::*)(Ts...) & noexcept) -> thunk<Ts...>;

template<typename T, typename... Ts>
thunk(T&, void (T::*)(Ts...)) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(T&, void (T::*)(Ts...) noexcept) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(T&, void (T::*)(Ts...) &) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(T&, void (T::*)(Ts...) & noexcept) -> thunk<Ts...>;

template<typename T, typename... Ts>
thunk(const T*, void (T::*)(Ts...) const) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(const T*, void (T::*)(Ts...) const noexcept) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(const T*, void (T::*)(Ts...) const&) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(const T*, void (T::*)(Ts...) const& noexcept) -> thunk<Ts...>;

template<typename T, typename... Ts>
thunk(const T&, void (T::*)(Ts...) const) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(const T&, void (T::*)(Ts...) const noexcept) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(const T&, void (T::*)(Ts...) const&) -> thunk<Ts...>;
template<typename T, typename... Ts>
thunk(const T&, void (T::*)(Ts...) const& noexcept) -> thunk<Ts...>;

namespace detail {
struct signal_block_base {
    virtual void do_disconnect(const void*) = 0;
    virtual ~signal_block_base() {}
};
}; // namespace detail

class observer {
    template<typename... Ts>
    friend class signal;

    mutable vector<pair<weak_ptr<detail::signal_block_base>, const void*>>
        deleters;

public:
    observer() {};

protected:
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
    void operator()(Args&&... args) const;

    // post an event to the internal queue.
    template<typename... Args>
    void post(Args&&... args);

    // flush the event queue to all slots.
    void flush();

    // connects a pointer to member function and an associated instance.
    template<typename T, typename M>
        requires std::invocable<M T::*, T*, Ts...>
    void connect(T* obj, M T::*mem_ptr)
    {
        block->do_connect(thunk<Ts...>{obj, mem_ptr});

        if constexpr (std::derived_from<T, observer>) {
            static_cast<observer*>(obj)->deleters.emplace_back(block, obj);
        }
    }

    template<typename T, typename M>
        requires std::invocable<M T::*, const T*, Ts...>
    void connect(const T* obj, M T::*mem_ptr)
    {
        block->do_connect(thunk<Ts...>{obj, mem_ptr});

        if constexpr (std::derived_from<T, observer>) {
            static_cast<const observer*>(obj)->deleters.emplace_back(block,
                                                                     obj);
        }
    }

    template<typename T, typename M>
        requires std::invocable<M T::*, T*, Ts...>
    void connect(T& obj, M T::*mem_ptr)
    {
        connect(std::addressof(obj), mem_ptr);
    }

    template<typename T, typename M>
        requires std::invocable<M T::*, const T*, Ts...>
    void connect(const T& obj, M T::*mem_ptr)
    {
        connect(std::addressof(obj), mem_ptr);
    }

    // connects a function pointer.
    void connect(void (*fun_ptr)(Ts...))
    {
        block->do_connect(thunk<Ts...>{fun_ptr});
    }
    void connect(void (*fun_ptr)(Ts...) noexcept)
    {
        block->do_connect(thunk<Ts...>{fun_ptr});
    }

    // connects a callable object.
    template<typename C>
    void connect(C* callable)
    {
        block->do_connect(thunk<Ts...>{callable});

        if constexpr (std::derived_from<C, observer>) {
            static_cast<observer*>(callable)->deleters.emplace_back(block,
                                                                    callable);
        }
    }

    template<typename C>
    void connect(const C* callable)
    {
        block->do_connect(thunk<Ts...>{callable});

        if constexpr (std::derived_from<C, observer>) {
            static_cast<const observer*>(callable)->deleters.emplace_back(
                block, callable);
        }
    }
    template<typename C>
    void connect(C& callable)
    {
        connect(std::addressof(callable));
    }
    template<typename C>
    void connect(const C& callable)
    {
        connect(std::addressof(callable));
    }

    // member function disconnect
    template<typename T, typename M>
    void disconnect(const T* obj, M T::*mem_ptr)
    {
        block->do_disconnect(obj, mem_ptr);
    }
    template<typename T, typename M>
    void disconnect(const T& obj, M T::*mem_ptr)
    {
        block->do_disconnect(std::addressof(obj), mem_ptr);
    }

    // function pointer disconnect.
    void disconnect(void (*fun_ptr)(Ts...))
    {
        block->do_disconnect((void*)fun_ptr);
    }
    void disconnect(void (*fun_ptr)(Ts...) noexcept)
    {
        block->do_disconnect((void*)fun_ptr);
    }

    // disconnects a callable or all members of an instance
    template<typename T>
    void disconnect(const T* obj)
    {
        block->do_disconnect(obj);
    }
    template<typename T>
    void disconnect(const T& obj)
    {
        block->do_disconnect(std::addressof(obj));
    }

    void disconnect_all() { block->slots.clear(); }

private:
    static_assert(sizeof(void(signal::*)(void)) == sizeof(uint128_t),
                  "pointer to member unexpected size");

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

        void do_disconnect(const void* ptr, uint128_t mptr)
        {
            auto [b, e] = ranges::equal_range(slots, pair{(void*)ptr, mptr});
            slots.erase(b, e);
        }

        template<typename T, typename M>
        void do_disconnect(const void* ptr, M T::*mptr)
        {
            auto m = bit_cast<uint128_t>(mptr);
            do_disconnect(ptr, mptr);
        }
    };

    shared_ptr<signal_block> block = std::make_shared<signal_block>();
};

template<typename... Ts>
template<typename... Args>
void signal<Ts...>::operator()(Args&&... args) const
{
    for (auto&& fn : block->slots) {
        fn(std::forward<Args>(args)...);
    }
}

template<typename... Ts>
template<typename... Args>
void signal<Ts...>::post(Args&&... args)
{
    block->evqueue.emplace_back(args...);
}

template<typename... Ts>
void signal<Ts...>::flush()
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

}; // namespace hera

#endif
