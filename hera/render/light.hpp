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

#ifndef HERA_RENDER_LIGHT_HPP
#define HERA_RENDER_LIGHT_HPP

#include <hera/common.hpp>
#include <hera/gl/buffer.hpp>
#include <hera/gl/program.hpp>

namespace hera {

class Light {
    vec3 pos;
    vec3 color;
    float ambient = 0.2;
    gl::VertexBuffer vbuf;
    mat4 model{1.0};

public:
    Light(vec3 pos, vec3 color = {1, 1, 1});

    // draw the light source's actual geometry.
    void draw(const gl::Pipeline&, float) const;

    // load light parameters into given shader.
    void load_into(const gl::Pipeline&) const;
};
} // namespace hera

#endif
