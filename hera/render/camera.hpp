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

#ifndef HERA_RENDER_CAMERA_HPP
#define HERA_RENDER_CAMERA_HPP

#include <hera/common.hpp>
#include <hera/gl/program.hpp>
#include <hera/input.hpp>
#include <hera/event.hpp>

namespace hera {

class Camera : public Observer {
    // view, projection, ortho_proj, view_pos
    gl::UniformBuffer<mat4, mat4, mat4, vec3> matblock;

    static constexpr size_t view_idx = 0;
    static constexpr size_t proj_idx = 1;
    static constexpr size_t ortho_idx = 2;
    static constexpr size_t pos_idx = 3;

    inline static const vec3 unit_front{0, 0, -1};
    inline static const vec3 unit_right{1, 0, 0};
    inline static const vec3 unit_up{0, 1, 0};

    vec3 _pos{0, 0, 4};

    // pitch, yaw, roll
    vec3 _angles = {0, 0, 0};

    float _speed = 5.0;
    vec2 _sensitivity = {60.0, 60.0};

    float _fov = 45.0;
    float _znear = 0.1;
    float _zfar = 100.0;

    ivec3 _trans_state{0, 0, 0};
    float _roll_state = 0;

    vec2 _fbsize = input::framebuffer_size();
    bool _proj_dirty = true;
    bool _ortho_dirty = true;

    mat4 _view;
    mat4 _proj;
    mat4 _ortho;

public:
    Camera() : matblock{"Matrices"}
    {
        input::actions.connect<&Camera::on_action>(this);
        input::cursor.connect<&Camera::on_cursor>(this);
        input::scroll.connect<&Camera::on_scroll>(this);
        input::fbsize.connect<&Camera::on_fbsize>(this);
    };

    void update();

    void load_into(gl::Shaders&) const;

    void on_action(input_action);
    void on_cursor(vec2, vec2);
    void on_scroll(vec2);
    void on_fbsize(ivec2);
};
} // namespace hera

#endif
