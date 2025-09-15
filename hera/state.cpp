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

#include <random>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_interpolation.hpp>

#include <hera/state.hpp>
#include <hera/event.hpp>
#include <hera/ui.hpp>

using hera::Cube;

namespace hera {

/*
namespace {
constexpr pair<float, float> circle(float angle, float radius)
{
    return {radius * sin(angle), radius * cos(angle)};
}
} // namespace
*/

void State::run()
{
    prologue();
    while (!input::should_close()) {
        preamble();
        loop();
        postamble();
    }
    epilogue();
}

void State::loop()
{
    while (ticker.pop()) {
        do_update();
        update_steps++;
    }
    do_render();
    render_steps++;
    do_input();
}

// one update tick
void State::do_update()
{
    camera->update();
    for (auto& cube : cubes) {
        cube.update();
    }
}

void State::do_render()
{
    Frame frame{*renderer};
    const float delta = ticker.delta();

    auto&& p = frame->pipeline("scene");
    dir_light.load_into("dir_light", p);
    for (auto i = 0u; i < plights.size(); ++i) {
        plights[i].load_into(std::format("point_lights[{}]", i), p);
    }
    int npl = plights.size();
    p.uniform("n_point_lights", npl);
    for (const auto& cube : cubes) {
        cube.draw(frame, delta);
    }

    frame->pipeline("lamp");
    for (const auto& pl : plights) {
        pl.draw(frame, delta);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void State::do_input()
{
    input::poll();
    input::flush();
}

void State::on_action(input_action act)
{
    using action = input_action::value;
    switch (act) {
    case action::escape:
        input::should_close(true);
        break;
    default:
        break;
    }
}

void State::prologue()
{
    gl::checkerror();
    Cube mastercube{"data:/container2.png", "data:/container2_specular.png"};
    gl::checkerror();

    std::mt19937 rgen{std::random_device{}()};
    std::uniform_real_distribution<float> roffset{0.0, numbers::pi * 2.0};
    std::uniform_real_distribution<float> runity{0.0, 1.0};

    auto randvec3 = [&]() {
        return vec3(runity(rgen), runity(rgen), runity(rgen));
    };

    auto randoff = [&]() { return roffset(rgen); };

    cube_pos.assign(
        {glm::vec3{0.0, 0.0, 0.0}, glm::vec3(2.0f, 5.0f, -15.0f),
         glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
         glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
         glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(1.5f, 2.0f, -2.5f),
         glm::vec3(1.5f, 0.2f, -1.5f), glm::vec3(-1.3f, 1.0f, -1.5f)});

    for (const auto& pos : cube_pos) {
        auto cube = mastercube;
        cube.position(pos);
        cube.axis(randvec3());
        cube.offset(randoff());
        cubes.push_back(std::move(cube));
    }

    plights.push_back(PointLight{{0.7, 0.2, 2.0}});
    plights.push_back(PointLight{{2.3, -3.3, -4.0}});
    plights.push_back(PointLight{{-4.0, 2.0, -12.0}});
    plights.push_back(PointLight{{0.0, 0.0, -3.0}});

    do_input();
    gl::checkerror();
    renderer->pipeline("scene");
    gl::checkerror();
    ticker.prev = clock::now();
}

void State::epilogue() {}

void State::preamble()
{
    gl::checkerror();
    // timestep
    ticker.push();
}

void State::postamble()
{
    gl::checkerror();
    auto now_time = clock::now();
    duration<float> diff = now_time - last_stat;
    if (diff > 1s) {
        /*
         * LOG_DEBUG("rps: {}", render_steps / diff.count());
         * LOG_DEBUG("ups: {}", update_steps / diff.count());
         */
        last_stat = now_time;
        render_steps = update_steps = 0;
    }
}
} // namespace hera
