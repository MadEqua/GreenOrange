#pragma once

#include <imgui.h>

#include "../gl/PreviewRenderer.h"

class Project;


class PreviewPanel
{
public:
    PreviewPanel();

    void drawGui(Project &project);

private:
    PreviewRenderer previewRenderer;

    ImVec2 previousSize;

    ImVec2 imagePos;
    ImVec2 imageSize;
};

