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

#ifndef HERA_WINDOW_HPP
#define HERA_WINDOW_HPP

#include <hera/common.hpp>
#include <hera/error.hpp>

namespace hera {

static constexpr size_t MAX_USERPTRS = 4;

template<typename T>
void register_userptr(T* ptr, GLFWwindow* window = glfwGetCurrentContext())
{
    auto userptrs = (uintptr_t*)glfwGetWindowUserPointer(window);
    for (auto i = 0uz; i < MAX_USERPTRS; ++i) {
        if (userptrs[i * 2uz] == 0) {
            userptrs[i * 2uz] = typeid(T).hash_code();
            userptrs[i * 2uz + 1uz] = (uintptr_t)ptr;
            return;
        }
    }
    throw hera::length_error("cannot exceed MAX_USERPTRS");
}

template<typename T>
T* get_userptr(GLFWwindow* window = glfwGetCurrentContext())
{
    auto userptrs = (uintptr_t*)glfwGetWindowUserPointer(window);
    for (auto i = 0uz; i < MAX_USERPTRS; ++i) {
        if (userptrs[i * 2] == typeid(T).hash_code()) {
            return reinterpret_cast<T*>(userptrs[i * 2 + 1]);
        }
    }
    throw hera::invalid_argument("type not found in userptrs");
}
} // namespace hera

#endif
