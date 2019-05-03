#pragma once

#include <GLFW/glfw3.h>

#include "ScenePanel.h"
#include "ProjectPanel.h"
#include "InspectorPanel.h"
#include "../model/GreenOrange.h"
#include "GuiSharedData.h"

class GuiRoot
{
public:
    GuiRoot(GreenOrange &greenOrange);

    bool init();
    void deinit();

    void run();
    void drawGui(int windowWidth, int windowHeight);

private:
    bool initGlfw();
    void deinitGlfw();

    bool initGlad();

    bool initImGui();
    void deinitImGui();

    GreenOrange &greenOrange;
    GLFWwindow *glfwWindow;
    
    ProjectPanel projectPanel;
    ScenePanel scenePanel;
    InspectorPanel inspectorPanel;

    GuiSharedData guiSharedData;
};