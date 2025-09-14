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

#include <hera/common.hpp>
#include <hera/gl/common.hpp>
#include <hera/init.hpp>
#include <hera/error.hpp>

namespace hera {

void init::gl()
{
    LOG_DEBUG("init GL");
    if (!gladLoadGL(glfwGetProcAddress)) {
        throw hera::runtime_error("failed to initialize GLAD");
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LOG_INFO("{:15} {}", "GL vendor:", gl::get<string_view>(GL_VENDOR));
    LOG_INFO("{:15} {}", "GL renderer:", gl::get<string_view>(GL_RENDERER));
    LOG_INFO("{:15} {}", "GL version:", gl::get<string_view>(GL_VERSION));
    LOG_INFO("{:15} {}", "GLSL version:",
             gl::get<string_view>(GL_SHADING_LANGUAGE_VERSION));

    LOG_DEBUG("init GL done");
}

} // namespace hera
