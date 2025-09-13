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

#ifndef HERA_RENDER_LIGHT_HPP
#define HERA_RENDER_LIGHT_HPP

#include <hera/common.hpp>
#include <hera/gl/buffer.hpp>
#include <hera/gl/program.hpp>
#include <hera/render/renderer.hpp>

namespace hera {

namespace detail {
struct light_vertex {
    float pos[3];
};

static constexpr light_vertex light_vertices[] = {
    {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, -0.5f},   {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},

    {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f},    {-0.5f, 0.5f, 0.5f},  {-0.5f, -0.5f, 0.5f},

    {-0.5f, 0.5f, 0.5f},   {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},

    {0.5f, 0.5f, 0.5f},    {0.5f, 0.5f, -0.5f},  {0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},

    {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f},
    {0.5f, -0.5f, 0.5f},   {-0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, -0.5f},

    {-0.5f, 0.5f, -0.5f},  {0.5f, 0.5f, -0.5f},  {0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f},    {-0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, -0.5f}};
} // namespace detail

template<>
struct gl::vertex<detail::light_vertex> : attributes<float[3]> {};

struct PointLight : public Drawable {
    vec3 position = {0, 0, 0};

    float constant = 1.0;
    float linear = 0.14;
    float quadratic = 0.07;

    vec3 ambient = {.05, .05, .05};
    vec3 diffuse = {.8, .8, .8};
    vec3 specular = {1, 1, 1};
    gl::VertexBuffer vbuf{detail::light_vertices};
    mat4 model{1.0};

    PointLight(vec3 pos) : position{pos}
    {
        model = glm::translate(model, pos);
        model = glm::scale(model, vec3{0.2});
    };

    // draw the light source's actual geometry.
    void draw(Frame& f, float delta) const override;

    // load light parameters into given shader.
    void load_into(const string& root, const gl::Pipeline&) const;
};

struct DirLight {
    vec3 direction;
    vec3 ambient = {.1, .1, .1};
    vec3 diffuse = {.4, .4, .4};
    vec3 specular = {.5, .5, .5};

    DirLight(vec3 dir) : direction{dir} {};

    void load_into(const string& root, const gl::Pipeline&) const;
};

} // namespace hera

#endif
