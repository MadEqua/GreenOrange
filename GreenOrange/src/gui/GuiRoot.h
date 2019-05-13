#pragma once

#include <vector>
#include <memory>

#include <GLFW/glfw3.h>

#include "Panel.h"

class GreenOrange;


class GuiRoot
{
public:
    GuiRoot(GreenOrange &greenOrange, GLFWwindow &glfwWindow);

    bool init();
    void deinit();

    void drawFrame();
private:
    GreenOrange &greenOrange;
    GLFWwindow &glfwWindow;

    std::vector<std::unique_ptr<Panel>> panels;

    void drawGui();
    void drawMenuBar();
};