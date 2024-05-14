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

namespace hera::render {

class Cube {
    gl::VertexBuffer vbuf;
    gl::Texture tex;
    gl::Texture spec;
    mat4 _model{1.0f};
    mat4 _prev_model{1.0f};

public:
    Cube(const path& texpath, const path& spec);

    glm::mat4& model() { return _model; }
    const glm::mat4& model() const { return _model; }
    glm::mat4& model(const glm::mat4& nm)
    {
        _prev_model = std::exchange(_model, nm);
        return _model;
    }

    glm::mat4 terp_model(float alpha) const
    {
        return glm::interpolate(_prev_model, _model, alpha);
    }

    void draw(const gl::Pipeline& shader, float alpha) const
    {
        shader.uniform("a_texture", 0);
        shader.uniform("spec_map", 1);

        auto mod = terp_model(alpha);
        shader.uniform("model", mod);

        tex.bind(0);
        spec.bind(1);

        vbuf.draw();
    }
};

} // namespace hera::render

#endif
