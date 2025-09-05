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

#ifndef HERA_RENDER_GEOMETRY_HPP
#define HERA_RENDER_GEOMETRY_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_interpolation.hpp>

#include <hera/common.hpp>
#include <hera/gl/vertex.hpp>
#include <hera/gl/buffer.hpp>
#include <hera/gl/program.hpp>
#include <hera/render/renderer.hpp>

namespace hera {

namespace {
struct quad_vertex {
    vec3 pos;
    vec2 tex;
};

static constexpr quad_vertex basic_quad_vertices[] = {
    {{1, 1, 0}, {1, 1}},     // top right
    {{1, -1, 0}, {1, -1}},   // bottom right
    {{-1, -1, 0}, {-1, -1}}, // bottom left
    {{-1, 1, 0}, {-1, 1}},   // top left
};

static constexpr uint16_t basic_quad_indices[] = {0, 1, 3, 1, 2, 3};

} // namespace

class Geometry : Drawable {
private:
    mat4 _model{1.0f};
    mat4 _prev_model{1.0f};

protected:
    gl::VertexBuffer _vbuf;

    Geometry() = default;
    Geometry(const gl::VertexBuffer& vbuf) : _vbuf{vbuf} {}

public:
    void draw(Frame& f, float alpha) const override;

    const mat4& model() const { return _model; }
    void model(const mat4& model)
    {
        _prev_model = std::exchange(_model, model);
    }

private:
    mat4 interpolate(float alpha) const
    {
        return glm::interpolate(_prev_model, _model, alpha);
    }
};

template<>
struct gl::vertex<quad_vertex> : attributes<vec3, vec2> {};

} // namespace hera

#endif
