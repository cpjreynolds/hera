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

#include <hera/image.hpp>
#include <hera/error.hpp>

namespace hera {

cached_type_t<image_data> asset<image_data>::load_from(const link& p)
{
    LOG_DEBUG("loading image: {}", p);
    // ivec2 size;
    // int channels;
    auto obj = std::make_shared_for_overwrite<image_data>();
    auto data = stbi_load(p.resolve().c_str(), &obj->size.x, &obj->size.y,
                          &obj->channels, 0);
    if (!data) {
        LOG_ERROR("stbi error: {}", stbi_failure_reason());
        throw runtime_error{"stbi error"};
    }
    obj->buf.reset(data);
    return obj;
    // return shared_ptr<image_data>{
    // new image_data{.buf{data}, .size = size, .channels = channels}};
}

} // namespace hera
