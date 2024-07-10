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

#ifndef HERA_RENDER_CUBE_HPP
#define HERA_RENDER_CUBE_HPP

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_interpolation.hpp>

#include <hera/common.hpp>
#include <hera/gl/buffer.hpp>
#include <hera/gl/texture.hpp>
#include <hera/gl/program.hpp>
#include <hera/render/geometry.hpp>

namespace hera {

class Cube : public Geometry {
    gl::Texture2d tex;
    gl::Texture2d spec;

public:
    Cube(const path& texpath, const path& spec);

    void draw(const gl::Pipeline& shader, float alpha) const override
    {
        shader.uniform("a_texture", 0);
        shader.uniform("spec_map", 1);

        auto mod = interpolate(alpha);
        shader.uniform("model", mod);

        tex.bind(0);
        spec.bind(1);

        _vbuf.draw();
    }
};

} // namespace hera

#endif
