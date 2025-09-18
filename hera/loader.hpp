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

// low-level byte-cache interface
class byte_cache {
public:
    using buffer_type = vector<uint8_t>;
    using value_type = shared_ptr<const buffer_type>;

    // const to indicate thread safety
    virtual value_type load(const link&) const = 0;

    virtual ~byte_cache() = default;
};

class file_cache : public byte_cache {
public:
    value_type load(const link& p) const override;

    static file_cache* get();

private:
    struct entry {
        value_type buf;
        fs::file_time_type mtime;
    };

    mutable hash_map<path, entry> cache;
    mutable shared_mutex mtx;
};

template<typename T>
struct importer {
    // load from bytes.
    static T load_from(span<const uint8_t>)
    {
        static_assert(false, "must specialize decoder::load");
    }

    static T load_from(const link&)
    {
        static_assert(false, "must specialize decoder::load");
    }
};

template<typename T, typename From>
concept loadable_from = requires(From from) {
    { importer<T>::load_from(from) } -> same_as<T>;
};

template<typename T>
concept loadable_from_bytes = loadable_from<T, span<const uint8_t>>;

template<typename T>
concept loadable = loadable_from_bytes<T> || loadable_from<T, path>;

template<typename T>
class asset_cache {
    byte_cache* bcache;
    mutable hash_map<path, weak_ptr<T>> cache;
    mutable shared_mutex mtx;

public:
    using value_type = shared_ptr<T>;

    explicit asset_cache(byte_cache* bc) : bcache{bc} {}

    shared_ptr<T> load(const link& pat) const
    {
        auto key = pat.resolve();

        // cache check
        {
            shared_lock lk{mtx};
            if (auto it = cache.find(key); it != cache.end()) {
                // cache hit
                if (auto ptr = it->second.lock(); ptr) {
                    // valid hit
                    LOG_DEBUG("asset_cache hit: {}", key);
                    return ptr;
                }
            }
        }
        // missing or expired
        LOG_DEBUG("asset_cache load: {}", key);
        return load_into(pat);
    }

private:
    shared_ptr<T> load_into(const link& pat) const
        requires loadable_from_bytes<T>
    {
        // load through bcache
        auto bytes = bcache->load(pat);
        shared_ptr<T> ptr{new T{importer<T>::load_from(*bytes)}};
        unique_lock lk{mtx};
        cache[pat.resolve()] = ptr;
        return ptr;
    }

    shared_ptr<T> load_into(const link& lnk) const
        requires(!loadable_from_bytes<T>)
    {
        // load from filepath
        shared_ptr<T> ptr{new T{importer<T>::load_from(lnk)}};
        unique_lock lk{mtx};
        cache[lnk.resolve()] = ptr;
        return ptr;
    }
};

class Assets {
public:
    template<typename T>
    static shared_ptr<T> load(const path& pat)
    {
        return get_cache<T>().load(pat);
    }

private:
    template<typename T>
    static const asset_cache<T>& get_cache()
    {
        static asset_cache<T> cache{file_cache::get()};
        return cache;
    }
};

struct image_data {
    unique_ptr<uint8_t, decltype(&free)> buf;
    ivec2 size;
    int channels;

    size_t size_bytes() const { return 1uz * size.x * size.y * channels; }

    span<uint8_t> operator*() const { return span{buf.get(), size_bytes()}; }
};

template<>
struct importer<image_data> {
    static image_data load_from(const link& p);
    static image_data load_from(span<const uint8_t> bytes);
};

} // namespace hera

#endif
