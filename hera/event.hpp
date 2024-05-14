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

namespace hera {

namespace detail {
template<typename T>
struct member_traits {
    using class_type = void;
    using fn_type = void;
};

template<typename T, typename U>
struct member_traits<T U::*> {
    using class_type = U;
    using fn_type = T;
    using type = T U::*;
};

template<typename T>
using member_class_t = member_traits<T>::class_type;

template<typename C, typename F>
concept member_fn_of =
    std::is_member_function_pointer_v<F> && same_as<member_class_t<F>, C>;
} // namespace detail

class observer {
    mutable hash_map<uintptr_t,
                     pair<weak_ptr<void>, void (*)(void*, observer*)>>
        deleters;

    template<typename... Ts>
    friend class signal;

public:
    observer() {};

protected:
    ~observer()
    {
        for (auto&& [key, del] : deleters) {
            auto [wptr, fn] = del;

            if (auto ptr = wptr.lock()) {
                fn(ptr.get(), this);
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
    template<typename T>
    void connect(T& obj, void (T::*mem_ptr)(Ts...));
    template<typename T>
    void connect(T& obj, void (T::*mem_ptr)(Ts...) noexcept);
    template<typename T>
    void connect(T& obj, void (T::*mem_ptr)(Ts...) &);
    template<typename T>
    void connect(T& obj, void (T::*mem_ptr)(Ts...) & noexcept);
    template<typename T>
    void connect(const T& obj, void (T::*mem_ptr)(Ts...) const);
    template<typename T>
    void connect(const T& obj, void (T::*mem_ptr)(Ts...) const noexcept);
    template<typename T>
    void connect(const T& obj, void (T::*mem_ptr)(Ts...) const&);
    template<typename T>
    void connect(const T& obj, void (T::*mem_ptr)(Ts...) const& noexcept);

    template<typename T>
    void connect(T* obj, void (T::*mem_ptr)(Ts...));
    template<typename T>
    void connect(T* obj, void (T::*mem_ptr)(Ts...) noexcept);
    template<typename T>
    void connect(T* obj, void (T::*mem_ptr)(Ts...) &);
    template<typename T>
    void connect(T* obj, void (T::*mem_ptr)(Ts...) & noexcept);
    template<typename T>
    void connect(const T* obj, void (T::*mem_ptr)(Ts...) const);
    template<typename T>
    void connect(const T* obj, void (T::*mem_ptr)(Ts...) const noexcept);
    template<typename T>
    void connect(const T* obj, void (T::*mem_ptr)(Ts...) const&);
    template<typename T>
    void connect(const T* obj, void (T::*mem_ptr)(Ts...) const& noexcept);

    // connects a function pointer.
    void connect(void (*fun_ptr)(Ts...));

    // connects a callable object.
    template<typename C>
    void connect(C& callable);
    template<typename C>
    void connect(const C& callable);
    template<typename C>
    void connect(C* callable);
    template<typename C>
    void connect(const C* callable);

    // member function disconnect

    template<typename T>
    void disconnect(const T& obj, void (T::*mem_ptr)(Ts...));
    template<typename T>
    void disconnect(const T& obj, void (T::*mem_ptr)(Ts...) noexcept);
    template<typename T>
    void disconnect(const T& obj, void (T::*mem_ptr)(Ts...) &);
    template<typename T>
    void disconnect(const T& obj, void (T::*mem_ptr)(Ts...) & noexcept);
    template<typename T>
    void disconnect(const T& obj, void (T::*mem_ptr)(Ts...) const);
    template<typename T>
    void disconnect(const T& obj, void (T::*mem_ptr)(Ts...) const noexcept);
    template<typename T>
    void disconnect(const T& obj, void (T::*mem_ptr)(Ts...) const&);
    template<typename T>
    void disconnect(const T& obj, void (T::*mem_ptr)(Ts...) const& noexcept);

    template<typename T>
    void disconnect(const T* obj, void (T::*mem_ptr)(Ts...));
    template<typename T>
    void disconnect(const T* obj, void (T::*mem_ptr)(Ts...) noexcept);
    template<typename T>
    void disconnect(const T* obj, void (T::*mem_ptr)(Ts...) &);
    template<typename T>
    void disconnect(const T* obj, void (T::*mem_ptr)(Ts...) & noexcept);
    template<typename T>
    void disconnect(const T* obj, void (T::*mem_ptr)(Ts...) const);
    template<typename T>
    void disconnect(const T* obj, void (T::*mem_ptr)(Ts...) const noexcept);
    template<typename T>
    void disconnect(const T* obj, void (T::*mem_ptr)(Ts...) const&);
    template<typename T>
    void disconnect(const T* obj, void (T::*mem_ptr)(Ts...) const& noexcept);

    void disconnect(void (*fun_ptr)(Ts...));

    // disconnects a callable or all members of an instance
    template<typename T>
    void disconnect(const T& obj);
    template<typename T>
    void disconnect(const T* obj);

    void disconnect_all() { block->slots.clear(); }

private:
    static_assert(sizeof(void(signal::*)(void)) == sizeof(uint128_t),
                  "pointer to member unexpected size");

    struct signal_block {
        hash_multimap<uintptr_t, pair<function<void(Ts...)>, uint128_t>> slots;
        vector<tuple<Ts...>> evqueue;
    };

    shared_ptr<signal_block> block = std::make_shared<signal_block>();

    template<typename T, typename F>
        requires detail::member_fn_of<T, F>
    void do_connect(T& obj, F mem_ptr)
    {
        do_connect(std::addressof(obj), mem_ptr);
    }

    template<typename T, typename F>
        requires detail::member_fn_of<T, F>
    void do_connect(T* obj, F mem_ptr)
    {
        auto info = std::bit_cast<uint128_t>(mem_ptr);
        auto key = (uintptr_t)obj;
        block->slots.emplace(key, pair{[=](Ts&&... args) -> void {
                                           (obj->*mem_ptr)(
                                               std::forward<Ts>(args)...);
                                       },
                                       info});
        if constexpr (std::derived_from<T, observer>) {
            void (*fp)(void*, observer*) = [](void* blkp, observer* selfp) {
                signal_block* blk = static_cast<signal_block*>(blkp);
                auto key = (uintptr_t) static_cast<T*>(selfp);
                blk->slots.erase(key);
            };
            auto thiskey = (uintptr_t)block.get();
            static_cast<observer*>(obj)->deleters.emplace(
                thiskey, pair{std::static_pointer_cast<void>(block), fp});
        }
    }

    template<typename T, typename F>
        requires detail::member_fn_of<T, F>
    void do_connect(const T& obj, F mem_ptr)
    {
        do_connect(std::addressof(obj), mem_ptr);
    }

    template<typename T, typename F>
        requires detail::member_fn_of<T, F>
    void do_connect(const T* obj, F mem_ptr)
    {
        auto info = std::bit_cast<uint128_t>(mem_ptr);
        auto key = (uintptr_t)obj;
        block->slots.emplace(key, pair{[=](Ts&&... args) -> void {
                                           (obj->*mem_ptr)(
                                               std::forward<Ts>(args)...);
                                       },
                                       info});
        if constexpr (std::derived_from<T, observer>) {
            void (*fp)(void*, observer*) = [](void* blkp, observer* selfp) {
                signal_block* blk = static_cast<signal_block*>(blkp);
                auto key = (uintptr_t) static_cast<T*>(selfp);
                blk->slots.erase(key);
            };
            auto thiskey = (uintptr_t)block.get();
            static_cast<const observer*>(obj)->deleters.emplace(
                thiskey, pair{std::static_pointer_cast<void>(block), fp});
        }
    }

    template<typename T, typename F>
        requires detail::member_fn_of<T, F>
    void do_disconnect(const T* obj, F mem_ptr)
    {
        auto target = std::bit_cast<uint128_t>(mem_ptr);
        auto key = (uintptr_t)obj;

        auto [it, end] = block->slots.equal_range(key);
        for (; it != end; ++it) {
            if (it->second.second == target) {
                block->slots.erase(it);
                return;
            }
        }
    }
};

template<typename... Ts>
template<typename... Args>
void signal<Ts...>::operator()(Args&&... args) const
{
    auto funs = block->slots | views::values | views::keys;
    for (auto&& fn : funs) {
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
    auto funs = block->slots | views::values | views::keys;
    for (auto&& fn : funs) {
        for (auto&& ev : block->evqueue) {
            std::apply(fn, ev);
        }
    }
    block->evqueue.clear();
}

template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(T& obj, void (T::*mem)(Ts...))
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(T& obj, void (T::*mem)(Ts...) noexcept)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(T& obj, void (T::*mem)(Ts...) &)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(T& obj, void (T::*mem)(Ts...) & noexcept)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(const T& obj, void (T::*mem)(Ts...) const)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(const T& obj, void (T::*mem)(Ts...) const noexcept)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(const T& obj, void (T::*mem)(Ts...) const&)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(const T& obj, void (T::*mem)(Ts...) const& noexcept)
{
    do_connect(obj, mem);
}

template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(T* obj, void (T::*mem)(Ts...))
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(T* obj, void (T::*mem)(Ts...) noexcept)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(T* obj, void (T::*mem)(Ts...) &)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(T* obj, void (T::*mem)(Ts...) & noexcept)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(const T* obj, void (T::*mem)(Ts...) const)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(const T* obj, void (T::*mem)(Ts...) const noexcept)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(const T* obj, void (T::*mem)(Ts...) const&)
{
    do_connect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::connect(const T* obj, void (T::*mem)(Ts...) const& noexcept)
{
    do_connect(obj, mem);
}

template<typename... Ts>
void signal<Ts...>::connect(void (*fun_ptr)(Ts...))
{
    block->slots.emplace((uintptr_t)fun_ptr, pair{fun_ptr, 0});
}

template<typename... Ts>
template<typename C>
void signal<Ts...>::connect(C* callable)
{
    auto key = (uintptr_t)callable;
    block->slots.emplace(key, pair{[=](Ts&&... args) {
                                       return callable->operator()(
                                           std::forward<Ts>(args)...);
                                   },
                                   0});
}
template<typename... Ts>
template<typename C>
void signal<Ts...>::connect(const C* callable)
{
    auto key = (uintptr_t)callable;
    block->slots.emplace(key, pair{[=](Ts&&... args) {
                                       return callable->operator()(
                                           std::forward<Ts>(args)...);
                                   },
                                   0});
}
template<typename... Ts>
template<typename C>
void signal<Ts...>::connect(C& callable)
{
    connect(std::addressof(callable));
}
template<typename... Ts>
template<typename C>
void signal<Ts...>::connect(const C& callable)
{
    connect(std::addressof(callable));
}

template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T& obj, void (T::*mem)(Ts...))
{
    do_disconnect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T& obj, void (T::*mem)(Ts...) noexcept)
{
    do_disconnect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T& obj, void (T::*mem)(Ts...) &)
{
    do_disconnect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T& obj, void (T::*mem)(Ts...) & noexcept)
{
    do_disconnect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T& obj, void (T::*mem)(Ts...) const)
{
    do_disconnect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T& obj,
                               void (T::*mem)(Ts...) const noexcept)
{
    do_disconnect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T& obj, void (T::*mem)(Ts...) const&)
{
    do_disconnect(obj, mem);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T& obj,
                               void (T::*mem)(Ts...) const& noexcept)
{
    do_disconnect(obj, mem);
}

template<typename... Ts>
void signal<Ts...>::disconnect(void (*fun_ptr)(Ts...))
{
    block->slots.erase((uintptr_t)fun_ptr);
}

template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T* obj)
{
    auto target = (uintptr_t)obj;
    block->slots.erase(target);
}
template<typename... Ts>
template<typename T>
void signal<Ts...>::disconnect(const T& obj)
{
    disconnect(std::addressof(obj));
}

template<typename T>
struct filter_traits {

    static constexpr T max_flag;
    static constexpr T all_flags = (max_flag & (max_flag - 1));
    static constexpr int num_flags = std::bit_width(max_flag);

    template<typename Ev>
    static constexpr bool isset(const T& filt, const Ev& evt)
    {
        return filt & evt;
    }
};

}; // namespace hera

#endif
