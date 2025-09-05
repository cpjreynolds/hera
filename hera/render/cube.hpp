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
#include <hera/tick.hpp>
#include <hera/gl/buffer.hpp>
#include <hera/gl/texture.hpp>
#include <hera/gl/program.hpp>
#include <hera/render/geometry.hpp>
#include <hera/render/material.hpp>

namespace hera {

class Cube : public Geometry {
    static constexpr float tau = numbers::pi * 2.0;
    static constexpr float rotate_rate = 1.0 / 6;
    static constexpr float increment = tau * rotate_rate * tickrate();

    gl::Texture2d diff;
    gl::Texture2d spec;
    float shine;
    vec3 _pos;
    vec3 _axis;
    float _angle;
    float _offset;

public:
    Cube(const path& diff, const path& spec, const vec3& pos = vec3{0.0},
         const vec3& axis = vec3{1.0, 0.0, 0.0}, float offset = 0.0);

    void draw(Frame& f, float) const override;

    void update()
    {
        _angle += increment;
        _angle = fmod(_angle, tau);
        mat4 nmodel = glm::translate(mat4{1.0}, _pos);
        nmodel = glm::rotate(nmodel, sin(_angle + _offset), _axis);
        Geometry::model(nmodel);
    }

    void position(const vec3& pos) { _pos = pos; }
    const vec3& position() const { return _pos; }

    const vec3& axis() const { return _axis; }
    void axis(const vec3& ax) { _axis = ax; }

    float offset() const { return _offset; }
    void offset(float off) { _offset = off; }
};

} // namespace hera

#endif
