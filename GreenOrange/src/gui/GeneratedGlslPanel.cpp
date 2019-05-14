#include "GeneratedGlslPanel.h"

#include <imgui.h>
#include "../model/GreenOrange.h"
#include "../GlslGenerator.h"


bool GeneratedGlslPanel::internalDrawGui(const GreenOrange &greenOrange) {
    bool open;

    ImGui::Begin("Generated GLSL", &open);
    {
        ImGui::TextWrapped(GlslGenerator::getInstance().getGlslCode().c_str());
    }
    ImGui::End();

    return open;
}
