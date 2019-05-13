#pragma once

#include <GLFW/glfw3.h>

#include "ScenePanel.h"
#include "ProjectPanel.h"
#include "InspectorPanel.h"
#include "PreviewPanel.h"
#include "../model/GreenOrange.h"

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
    
    ProjectPanel projectPanel;
    ScenePanel scenePanel;
    InspectorPanel inspectorPanel;
    PreviewPanel previewPanel;

    void drawGui();
};