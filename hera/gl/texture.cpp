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

#include <hera/io/image.hpp>
#include <hera/gl/texture.hpp>

namespace hera::gl {

namespace {
internal_f channels_to_format(int channels)
{
    switch (channels) {
    case 1:
        return internal_f::red;
    case 2:
        return internal_f::rg;
    case 3:
        return internal_f::rgb;
    case 4:
        return internal_f::rgba;
    default:
        throw gl_error{"bad texture image channel number"};
    }
}
} // namespace

Texture2d::Texture2d(const link& fpath, const TextureParams& params,
                     texture_u unit)
    : Texture{unit}
{
    auto img = assets::get<image_data>(fpath);
    internal_f format = channels_to_format(img->channels);

    bind();
    params.apply(target);
    gl::allocate(target, format, img->size.x, img->size.y, **img);

    glGenerateMipmap(+target);
}

void Texture2d::allocate(const image_data& data, const TextureParams& params)
{
    bind();
    params.apply(target);
    gl::allocate(target, channels_to_format(data.channels), data.size.x,
                 data.size.y, *data);
    glGenerateMipmap(+target);
}

void Texture2d::allocate(const link& pat, const TextureParams& params)
{
    auto img = assets::get<image_data>(pat);
    allocate(*img, params);
}

} // namespace hera::gl
