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

#include <hera/render/camera.hpp>
#include <hera/tick.hpp>

namespace hera {

void Camera::on_action(input_action act)
{
    using action = input_action::value;
    float m = act.down() ? 1. : -1.;
    switch (act) {
    case action::move_fwd:
        _trans_state += m * unit_front;
        break;
    case action::move_back:
        _trans_state += m * -unit_front;
        break;
    case action::move_left:
        _trans_state += m * -unit_right;
        break;
    case action::move_right:
        _trans_state += m * unit_right;
        break;
    case action::move_up:
        _trans_state += m * unit_up;
        break;
    case action::move_down:
        _trans_state += m * -unit_up;
        break;
    case action::roll_right:
        _roll_state += m * -1;
        break;
    case action::roll_left:
        _roll_state += m * 1;
        break;
    case action::toggle_mode:
        if (act.down())
            input::toggle_cursor();
        break;
    default:
        break;
    }
}

void Camera::on_scroll(vec2 delta)
{
    _fov = std::clamp(_fov - (_sensitivity.y * delta.y), 1.0f, 85.0f);
    _proj_dirty = true;
}

void Camera::on_cursor(vec2 delta, vec2)
{
    _angles += vec3{_sensitivity * vec2{delta.y, delta.x}, 0};
}

void Camera::on_fbsize(ivec2 delta)
{
    _fbsize = {delta.x, delta.y};
    _ortho_dirty = _proj_dirty = true;
}

void Camera::update()
{
    using glm::quat;

    _angles.z += _roll_state;
    quat orient(glm::radians(_angles));
    float velocity = tickrate() * _speed;

    vec3 dir = orient * vec3{_trans_state};

    _pos += dir * velocity;
    matblock.write<pos_idx>(_pos);

    vec3 fwd = orient * unit_front;
    vec3 up = orient * unit_up;
    _view = glm::lookAt(_pos, _pos + fwd, up);

    matblock.write<view_idx>(_view);

    if (_proj_dirty) {
        _proj = glm::perspective<float>(glm::radians(_fov),
                                        _fbsize.x / _fbsize.y, _znear, _zfar);
        matblock.write<proj_idx>(_proj);
        _proj_dirty = false;
    }

    if (_ortho_dirty) {
        _ortho = glm::ortho<float>(0, _fbsize.x, 0, _fbsize.y);
        matblock.write<ortho_idx>(_ortho);
        _ortho_dirty = false;
    }
}

void Camera::load_into(gl::Shaders& shaders) const
{
    shaders.add_block(matblock);
}

} // namespace hera
