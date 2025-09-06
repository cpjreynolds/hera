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

#include <hera/ui.hpp>

#include <hera/common.hpp>
#include <hera/init.hpp>
#include <hera/input.hpp>
#include <hera/config.hpp>

namespace hera {

void init::ui()
{
    LOG_DEBUG("init ui");
    const char* glsl_version = "#version 330";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigNavEscapeClearFocusWindow = true;

    Config config{};

    path fontpath = path(config["assets.path"]) /
                    path(config["assets.fonts.path"]) /
                    path(config["assets.fonts.regular"]);

    auto font = io.Fonts->AddFontFromFileTTF(fontpath.native().c_str(), 14);
    io.FontDefault = font;

    ImGui::StyleColorsDark();

    auto window = glfwGetCurrentContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    LOG_DEBUG("init ui done");
}

void deinit::ui()
{
    LOG_DEBUG("deinit ui");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    LOG_DEBUG("deinit ui done");
}

} // namespace hera
