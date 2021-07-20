//
//  main.cpp
//  hera
//
//  Created by Cole Reynolds on 7/14/21.
//

#include <GLFW/glfw3.h>

#include <iostream>

#include "input.hpp"

void fb_resize_cb(GLFWwindow* win, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "hera", nullptr, nullptr);
    if (!window) {
        std::cerr << "failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, fb_resize_cb);
    
    glfwSetKeyCallback(window, key_input_cb);
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.5,0.5,0.5, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}
