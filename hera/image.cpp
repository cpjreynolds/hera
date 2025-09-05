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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <hera/error.hpp>
#include <hera/image.hpp>

namespace hera {
image_data load_image(const path& pat)
{
    ivec2 size;
    int channels;
    auto data = stbi_load(pat.c_str(), &size.x, &size.y, &channels, 0);
    if (!data) {
        throw hera::runtime_error("stbi error: "s + stbi_failure_reason());
    }
    return {.buf = unique_ptr<unsigned char, decltype(&free)>{data, &free},
            .size = size,
            .channels = channels};
}

image_data load_image(const unsigned char* buf, int len)
{
    ivec2 size;
    int channels;
    auto data = stbi_load_from_memory(buf, len, &size.x, &size.y, &channels, 0);
    if (!data) {
        throw hera::runtime_error("stbi error: "s + stbi_failure_reason());
    }
    return {.buf = unique_ptr<unsigned char, decltype(&free)>{data, &free},
            .size = size,
            .channels = channels};
}

} // namespace hera
