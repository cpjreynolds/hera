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

#ifndef HERA_STATE_HPP
#define HERA_STATE_HPP

#include <hera/common.hpp>
#include <hera/config.hpp>
#include <hera/init.hpp>
#include <hera/input.hpp>
#include <hera/tick.hpp>
#include <hera/render/cube.hpp>
#include <hera/render/light.hpp>
#include <hera/render/renderer.hpp>

namespace hera {

class State {
private:
    struct Private {
        explicit Private() = default;
    };

public:
    GLFWwindow* window;
    Config config;
    shared_ptr<Renderer> renderer = Renderer::create(config);
    Ticker ticker;

    long render_steps = 0;
    long update_steps = 0;

    clock::time_point last_stat = clock::now();

    // state data
    float angle = 0;
    // rotations per second
    const float cam_rotrate = 1.0 / 10;
    float camx = 0;
    float camz = 0;
    const float radius = 10;

    const float cube_rotrate = 1.0 / 6;
    float cube_angle = 0;

    // render data
    vector<Cube> cubes;
    vector<vec3> cube_pos;

    DirLight dir_light;
    vector<PointLight> plights;
    shared_ptr<Camera> camera = Camera::create();

    State(Private) : window{glfwGetCurrentContext()}, dir_light{{0, -1.0, 0}}
    {
        gl::checkerror();
        camera->load_into(renderer->shaders);
    };

    static shared_ptr<State> create()
    {
        auto self = std::make_shared<State>(Private{});
        input::actions.connect<&State::on_action>(self);
        return self;
    }

    void on_action(input_action);

    // runs the loop until exit.
    void run();

    // performs one iteration of the loop.
    void loop();

    void busy_sleep();

    // performs one render tick.
    void do_render();
    // performs one input tick.
    void do_input();
    // performs one update tick.
    void do_update();

    // run once before entire loop.
    void prologue();
    // run once after entire loop.
    void epilogue();

    // run once before each loop iteration.
    void preamble();
    // run once after each loop iteration.
    void postamble();
};
} // namespace hera
#endif
