#include "GeneratedGlslPanel.h"

#include <imgui.h>
#include "../model/GreenOrange.h"


bool GeneratedGlslPanel::internalDrawGui(const GreenOrange &greenOrange) {
    bool open;

    ImGui::Begin("Generated GLSL", &open);
    {
        ImGui::TextWrapped(greenOrange.getCurrentProjectGlsl().c_str());
    }
    ImGui::End();

    return open;
}
