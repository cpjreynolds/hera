//
//  input.cpp
//  hera
//
//  Created by Cole Reynolds on 7/16/21.
//

#include "input.hpp"

void key_input_cb(GLFWwindow* win, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(win, true);
    }
}
