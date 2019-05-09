#include "PreviewPanel.h"

#include <imgui.h>

const char* FS_TEMP = "#version 430 core\n"\
"out vec4 fragColor;"\
"void main() {"\
    "fragColor = vec4(gl_FragCoord.xy / vec2(800, 450), 0.0f, 1.0f);"\
"}";


PreviewPanel::PreviewPanel() :
    width(800),
    height(450), //TODO: set a good first size
    previewRenderer(static_cast<uint32>(width), static_cast<uint32>(height), FS_TEMP) {
}

void PreviewPanel::drawGui() {
    ImGui::Begin("Preview");
    {
        ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);

        float w = ImGui::GetWindowWidth();
        float h = ImGui::GetWindowHeight();
        if(w != width || h != height) {
            width = w;
            height = h;
            previewRenderer.setDimensions(static_cast<uint32>(w), static_cast<uint32>(h));
        }

        previewRenderer.render();
        ImGui::Image((ImTextureID)(void*)previewRenderer.getRenderedImageId(), ImVec2(w, h), ImVec2(0, 1), ImVec2(1, 0)); //TODO adjust size
    }
    ImGui::End();
}
