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

#include <hera/window.hpp>
#include <hera/render/renderer.hpp>
#include <hera/link.hpp>

namespace hera {

Renderer::Renderer(const Config&, Private) : _window{glfwGetCurrentContext()}
{
    LOG_DEBUG("init renderer");
    path shaderpath = link::apply("hera:shaders");
    shaders.load(shaderpath);
    LOG_INFO("{}", *this);
    LOG_DEBUG("init renderer done");
}

void Renderer::on_action(input_action act)
{
    if (act == input_action::reload && act.down()) {
        LOG_DEBUG("reloading shaders");
        // path shaderpath = Config{}["shaders.path"];
        // shaders.load(shaderpath);
        shaders.load();
    }
}

void Renderer::begin_frame()
{
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

const gl::Pipeline& Renderer::pipeline() const
{
    return shaders.active();
}

const gl::Pipeline& Renderer::pipeline(string_view name)
{
    auto&& p = shaders.active(name);
    return p;
}

} // namespace hera
