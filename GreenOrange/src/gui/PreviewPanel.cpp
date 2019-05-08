#include "PreviewPanel.h"

#include <imgui.h>

const char* FS_TEMP = "#version 430 core\n"\
"out vec4 fragColor;"\
"void main() {"\
    "fragColor = vec4(.3, .3, .1, 1.);"\
"}";


PreviewPanel::PreviewPanel() :
    previewRenderer(FS_TEMP) {
}

void PreviewPanel::drawGui() {
    previewRenderer.render();

    ImGui::Begin("Preview", 0, 0);
    {
        ImGui::Image((ImTextureID) previewRenderer.getRenderedImageId(), ImVec2(512, 512));
    }
    ImGui::End();
}
