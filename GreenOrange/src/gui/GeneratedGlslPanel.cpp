#include "GeneratedGlslPanel.h"

#include <imgui.h>
#include "../model/GreenOrange.h"
#include "../GlslGenerator.h"


bool GeneratedGlslPanel::internalDrawGui(const GreenOrange &greenOrange) {
    bool open;

    ImGui::Begin("Generated GLSL", &open);
    {
        const char *glsl = GlslGenerator::getInstance().getGlslCode().c_str();
        if(ImGui::Button("Copy")) {
            ImGui::SetClipboardText(glsl);
        }
        ImGui::TextWrapped(glsl);
    }
    ImGui::End();

    return open;
}
