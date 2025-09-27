// hera
// Copyright (C) 2024-2025  Cole Reynolds
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

#ifndef HERA_LOADER_HPP
#define HERA_LOADER_HPP

#include <hera/common.hpp>
#include <hera/io/link.hpp>

namespace hera {

/*
 * specialize and provide a `load_from(const link&)` function.
 */
template<typename T>
struct asset {
    /*
    cached_type load_from(const link&);
    */
};

template<typename T>
concept simple_asset = requires(asset<T> a, link l) {
    { a.load_from(l) } -> same_as<shared_ptr<T>>;
};

template<typename T>
concept weak_asset =
    simple_asset<T> && requires { typename asset<T>::weak_storage; };

template<typename T>
concept custom_asset = !simple_asset<T> && requires(asset<T> a, link l) {
    { a.load_from(l) };
};

template<typename T>
concept asset_type = simple_asset<T> || weak_asset<T> || custom_asset<T>;

template<typename T>
    requires asset_type<T>
struct asset_traits {
public:
    using cached_type =
        decltype(declval<asset<T>>().load_from(declval<link>()));

    using storage_type =
        std::conditional_t<weak_asset<T>, weak_ptr<T>, cached_type>;
};

template<typename T>
using cached_type_t = asset_traits<T>::cached_type;

template<typename T>
using storage_type_t = asset_traits<T>::storage_type;

template<typename T>
    requires asset_type<T>
class cache {
public:
    using cached_type = cached_type_t<T>;
    using storage_type = storage_type_t<T>;

    cache() = default;

    // retrieves an asset. loads if necessary.
    cached_type get(const link& pat) const
    {
        {
            shared_lock lk{mtx};
            if (auto elt = storage.find(*pat); elt != storage.end()) {
                // cache hit
                if constexpr (weak_asset<T>) {
                    if (auto obj = elt->second.lock(); obj) {
                        LOG_DEBUG("cache hit: {}", pat);
                        return obj;
                    }
                    else {
                        LOG_DEBUG("cache expired: {}", pat);
                    }
                }
                else {
                    LOG_DEBUG("cache hit: {}", pat);
                    return elt->second;
                }
            }
        }
        // miss
        return load(pat);
    }

    // loads an asset regardless of cache status
    cached_type load(const link& pat) const
    {
        asset<T> importer;
        cached_type obj = importer.load_from(pat);
        scoped_lock lk{mtx};
        storage[*pat] = obj;
        return obj;
    }

    // insert an object into the cache
    void put(const link& pat, cached_type obj)
    {
        scoped_lock lk{mtx};
        storage[*pat] = std::move(obj);
    }

private:
    mutable hash_map<path, storage_type> storage;
    mutable shared_mutex mtx;
};

class assets {
public:
    template<typename T>
    static cached_type_t<T> get(const link& pat)
    {
        return get_cache<T>().get(pat);
    }

    template<typename T>
    static void put(const link& pat, cached_type_t<T> obj)
    {
        get_cache<T>().put(pat, std::move(obj));
    }

private:
    template<typename T>
    static const cache<T>& get_cache()
    {
        static cache<T> cache{};
        return cache;
    }
};

} // namespace hera

#endif
