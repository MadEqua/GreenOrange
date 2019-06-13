#pragma once

#include <vector>
#include <memory>

#include <GLFW/glfw3.h>

#include "Panel.h"
#include "../Model/Entity.h"
#include "../Types.h"

class GreenOrange;


class GuiRoot
{
public:
    GuiRoot(GreenOrange &greenOrange, GLFWwindow &glfwWindow);

    bool init();
    void deinit();

    void drawFrame();

    void setSelectedEntity(Entity &ent) { selectedEntityId = ent.getId(); }
    Entity* getSelectedEntity();
    void clearSelectedEntity() { selectedEntityId = -1; }

private:
    GreenOrange &greenOrange;
    GLFWwindow &glfwWindow;

    std::vector<std::unique_ptr<Panel>> panels;

    uint32 selectedEntityId = -1;

    void drawGui();
    void drawMenuBar();
};