#pragma once

#include "Panel.h"
#include <imgui.h>

#include "../gl/PreviewRenderer.h"
#include "../Types.h"

class Project;
class GreenOrange;
class GuiRoot;


class PreviewPanel : public Panel
{
public:
    PreviewPanel(GuiRoot &guiRoot);

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;
    
    PreviewRenderer previewRenderer;

    ImVec2 previousSize;

    ImVec2 imagePos;
    ImVec2 imageSize;

    uint64 lastCodeId = -1;
};

