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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <hera/loader.hpp>
#include <hera/init.hpp>
#include <hera/config.hpp>
#include <hera/utility.hpp>

namespace hera {

namespace {
// 'domain' (assets:, shaders:, etc)
static hash_map<path, path> domains;
} // namespace

void init::loader()
{
    Config cfg;

    const toml::array& cfg_doms = cfg.at("domains");
    cfg_doms.for_each([](const toml::table& elt) {
        elt.at("domain").visit([&](const toml::value<string>& dom) {
            elt.at("path").visit([&](const toml::value<string>& pat) {
                domains.insert({*dom, *pat});
                LOG_DEBUG("registered domain: {} = {}", *dom, *pat);
            });
        });
    });
}

path path_resolver::current() const
{
    path rv = _root;
    for (const auto& pat : _patstack) {
        rv /= pat;
    }
    return rv;
}

path path_resolver::apply(const path& pat) const
{
    if (pat.empty())
        throw runtime_error{"empty path"};

    path normed = pat.lexically_normal();

    if (ranges::starts_with(normed, _root)) {
        // already a key
        return normed;
    }

    path result;
    path head = *normed.begin();

    if (head.native().back() == ':') {
        if (auto val = domains.find(head); val != domains.cend()) {
            result = val->second;
        }
        else {
            LOG_ERROR("invalid domain: {}", head);
            throw runtime_error{"invalid domain"};
        }
    }
    else {
        result = _root;
    }
    for (const auto& pat : _patstack) {
        result /= pat;
    }
    result /= normed;
    return result;
}

path_resolver& path_resolver::get()
{
    thread_local path_resolver globl{Config{}.at<path>("assets.path")};
    return globl;
}

// ====[file_cache]====

file_cache::value_type file_cache::load(const path& key) const
{
    auto cur_mtime = fs::last_write_time(key);

    // cache check
    {
        shared_lock lk{mtx};
        if (auto it = cache.find(key); it != cache.end()) {
            if (it->second.mtime == cur_mtime) {
                // no update
                LOG_DEBUG("file cache hit: {}", key);
                return it->second.buf;
            }
        }
    }
    // no dice. load it
    LOG_DEBUG("file_cache load: {}", key);
    buffer_type buf;
    slurp(key, buf, ios_base::binary);
    auto sptr = std::make_shared<const buffer_type>(std::move(buf));
    unique_lock lk{mtx};
    cache[key] = {.buf = sptr, .mtime = cur_mtime};
    return sptr;
}

file_cache* file_cache::get()
{
    static file_cache globl;
    return &globl;
}

image_data loader<image_data>::load_from(const path& p)
{
    LOG_DEBUG("loading image: {}", p);
    ivec2 size;
    int channels;
    auto data = stbi_load(p.c_str(), &size.x, &size.y, &channels, 0);
    if (!data) {
        LOG_ERROR("stbi error: {}", stbi_failure_reason());
        throw runtime_error{"stbi error"};
    }
    return {.buf = unique_ptr<uint8_t, decltype(&free)>{data, &free},
            .size = size,
            .channels = channels};
}

image_data loader<image_data>::load_from(span<const uint8_t> bytes)
{
    ivec2 size;
    int channels;
    auto data = stbi_load_from_memory(bytes.data(), bytes.size_bytes(), &size.x,
                                      &size.y, &channels, 0);
    if (!data) {
        LOG_ERROR("stbi error: {}", stbi_failure_reason());
        throw runtime_error{"stbi error"};
    }
    return {.buf = unique_ptr<uint8_t, decltype(&free)>{data, &free},
            .size = size,
            .channels = channels};
}

} // namespace hera
