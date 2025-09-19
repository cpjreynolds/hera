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
#include <hera/error.hpp>
#include <hera/link.hpp>

namespace hera {

template<typename T>
struct importer {
    static T load_from(const link&)
    {
        static_assert(false, "must specialize importer::load_from");
    }
};

template<typename T, typename From>
concept loadable_from = requires(From from) {
    { importer<T>::load_from(from) } -> same_as<T>;
};

template<typename T>
concept loadable = loadable_from<T, link>;

template<typename T>
class asset_cache {
    mutable hash_map<path, shared_ptr<T>> cache;
    mutable shared_mutex mtx;

public:
    using value_type = shared_ptr<T>;

    shared_ptr<T> load(const link& pat) const
    {
        // cache check
        {
            shared_lock lk{mtx};
            if (auto elt = cache.find(pat); elt != cache.cend()) {
                // cache hit
                LOG_DEBUG("asset_cache hit: {}", pat);
                return elt->second;
            }
        }
        // missing or expired
        return load_into(pat);
    }

private:
    shared_ptr<T> load_into(const link& pat) const
    {
        // load from filepath
        shared_ptr<T> ptr{new T{importer<T>::load_from(pat)}};
        unique_lock lk{mtx};
        cache[pat] = ptr;
        return ptr;
    }
};

class assets {
public:
    template<typename T>
    static shared_ptr<T> load(const link& pat)
    {
        return get_cache<T>().load(pat);
    }

private:
    template<typename T>
    static const asset_cache<T>& get_cache()
    {
        static asset_cache<T> cache{};
        return cache;
    }
};

struct image_data {
    struct deleter {
        void operator()(uint8_t* p) const { ::free(p); }
    };
    unique_ptr<uint8_t, deleter> buf;
    ivec2 size;
    int channels;

    size_t size_bytes() const { return 1uz * size.x * size.y * channels; }

    span<uint8_t> operator*() const { return span{buf.get(), size_bytes()}; }
};

template<>
struct importer<image_data> {
    static image_data load_from(const link& p);
};

} // namespace hera

#endif
