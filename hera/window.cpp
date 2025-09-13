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

#include <hera/log.hpp>
#include <hera/error.hpp>
#include <hera/init.hpp>
#include <hera/window.hpp>

namespace hera {

namespace {
uintptr_t window_userptrs[MAX_USERPTRS * 2]{};

void log_window_info(GLFWwindow* window)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int fbwidth, fbheight;
    int winwidth, winheight;
    glfwGetWindowSize(window, &winwidth, &winheight);
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

    int mm_wide, mm_high;
    float xscale, yscale;

    glfwGetMonitorPhysicalSize(monitor, &mm_wide, &mm_high);
    glfwGetWindowContentScale(window, &xscale, &yscale);

    int xdpmm = mode->width / mm_wide;
    int ydpmm = mode->height / mm_high;

    long xdpi = (127 * xdpmm) / 5;
    long ydpi = (127 * ydpmm) / 5;

    xdpi = std::lrint(float(xdpi) * xscale);
    ydpi = std::lrint(float(ydpi) * yscale);

    LOG_INFO("{:15} {}", "monitor name:", glfwGetMonitorName(monitor));
    LOG_INFO("{:15} {} Hz", "refresh rate:", mode->refreshRate);
    LOG_INFO("{:15} {}x{}", "monitor size:", mode->width, mode->height);
    LOG_INFO("{:15} {}x{}", "monitor dpi:", xdpi, ydpi);
    LOG_INFO("{:15} {}x{}", "content scale:", xscale, yscale);
    LOG_INFO("{:15} {}x{}", "window size:", winwidth, winheight);
    LOG_INFO("{:15} {}x{}", "fbuffer size:", fbwidth, fbheight);
}
} // namespace

GLFWwindow* init::window()
{
    LOG_DEBUG("init window");
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(DARWIN)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

    GLFWwindow* window = glfwCreateWindow(1200, 800, "hera", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw hera::runtime_error("failed to create window");
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, &window_userptrs);

#if HERA_APPLE_FUCKERY
    glfwSwapInterval(0);
#else
    glfwSwapInterval(1);
#endif

    log_window_info(window);
    LOG_DEBUG("init window done");
    return window;
}

void deinit::window()
{
    LOG_DEBUG("deinit window");
    auto window = glfwGetCurrentContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    LOG_DEBUG("deinit window done");
}

}; // namespace hera
