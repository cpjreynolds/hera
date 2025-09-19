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

#include <hera/image.hpp>
#include <hera/gl/texture.hpp>

namespace hera::gl {

Texture2d::Texture2d(const path& fpath, const TextureParams& params,
                     texture_u unit)
    : Texture{unit}
{
    auto img = assets::get<image_data>(fpath);
    internal_f format;
    switch (img->channels) {
    case 1:
        format = internal_f::red;
        break;
    case 2:
        format = internal_f::rg;
    case 3:
        format = internal_f::rgb;
        break;
    case 4:
        format = internal_f::rgba;
        break;
    default:
        throw gl_error{"bad texture image channel number"};
    }

    bind();
    params.apply(target);
    gl::allocate(target, format, img->size.x, img->size.y, **img);

    glGenerateMipmap(GL_TEXTURE_2D);
}

} // namespace hera::gl
