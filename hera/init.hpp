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

#ifndef HERA_INIT_HPP
#define HERA_INIT_HPP

#include <GLFW/glfw3.h>

namespace hera {

struct init {
    init() = delete;

    // declared in the order they need to be called.
    static void logging();
    static void error();
    static void config();
    static void link();
    static void loader();
    static GLFWwindow* window();
    static void gl();
    static void input();
    static void ui();
};

struct deinit {
    deinit() = delete;

    static void input();
    static void window();
    static void ui();
};

struct init_handle {
    init_handle()
    {
        init::logging();
        init::error();
        init::config();
        init::link();
        init::loader();
        init::window();
        init::gl();
        init::input();
        init::ui();
    }

    ~init_handle()
    {
        deinit::ui();
        deinit::input();
        deinit::window();
    }
};

} // namespace hera

#endif
