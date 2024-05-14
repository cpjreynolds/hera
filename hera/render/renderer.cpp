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

#include <glm/ext/matrix_clip_space.hpp>

#include <hera/log.hpp>
#include <hera/window.hpp>
#include <hera/render/renderer.hpp>

namespace hera::render {

Renderer::Renderer(const Config& config)
    : _window{glfwGetCurrentContext()},
      projector{config}
{
    LOG_DEBUG("init renderer");
    path shaderpath = config["shaders.path"];
    shaders.load(shaderpath);
    raw_input::actions.connect(*this, &Renderer::on_action);
    LOG_INFO("{}", fmt::to_string(*this));
    LOG_DEBUG("init renderer done");
}

void Renderer::on_action(input_action act)
{
    if (act == input_action::reload && act.down()) {
        LOG_DEBUG("reloading shaders");
        path shaderpath = Config{}["shaders.path"];
        shaders.load(shaderpath);
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

} // namespace hera::render
