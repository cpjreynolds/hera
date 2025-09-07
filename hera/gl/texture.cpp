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

#include <hera/image.hpp>
#include <hera/gl/texture.hpp>

namespace hera::gl {

Texture2d::Texture2d(const path& fpath, const TextureParams& params,
                     texture_u unit)
    : Texture{unit}
{
    if (!fs::exists(fpath)) {
        throw hera::runtime_error(string(fpath.filename()) + " does not exist");
    }
    LOG_INFO("loading texture file: {}", fpath);
    auto [buf, size, channels] = load_image(fpath);
    internal_f format;
    switch (channels) {
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
    size_t bytes = 1uz * size.x * size.y * channels;
    gl::allocate(target, format, size.x, size.y, span(buf.get(), bytes));

    glGenerateMipmap(GL_TEXTURE_2D);
}

} // namespace hera::gl
