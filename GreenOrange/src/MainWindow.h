#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>


class MainWindow
{
public:
    bool mainLoop();

    void draw();

private:
    const GLint OPENGL_MAJOR_VERSION = 4;
    const GLint OPENGL_MINOR_VERSION = 3;
    const char* GLSL_VERSION = "#version 430";

    const ImVec4 CLEAR_COLOR = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    int width, height;

    bool initOpenGL();

    bool initGlfw();
    void deinitGlfw();

    bool initImGui();
    void deinitImGui();

    GLFWwindow *glfwWindow;
};