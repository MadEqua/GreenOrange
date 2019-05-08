#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>


bool initGlfw();
void deinitGlfw();

bool initGlad();

extern GLFWwindow *glfwWindow;