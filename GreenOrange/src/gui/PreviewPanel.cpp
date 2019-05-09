#include "PreviewPanel.h"


const char* FS_TEMP = "#version 430 core\n"
"out vec4 fragColor;"
"void main() {"
    "fragColor = vec4(gl_FragCoord.xy / vec2(800, 450), 0.0f, 1.0f);"
    //"fragColor = vec4(gl_FragCoord.y > gl_FragCoord.x ? vec3(0) : vec3(1), 1.0f);"
"}";


PreviewPanel::PreviewPanel() :
    previousSize(-1.0f, 1.0f) {

    //TODO: set shader with every change
    previewRenderer.setFragmentShader(FS_TEMP);
}

void PreviewPanel::drawGui() {
    ImGui::Begin("Preview");
    {
        ImVec2 size = ImGui::GetContentRegionAvail();

        if(size.x > 0.0f && size.y > 0.0f) {
            if(size.x != previousSize.x || size.y != previousSize.y) {
                previousSize = size;

                float windowRatio = size.x / size.y;
                float scaleHeight = windowRatio * (9.0f / 16.0f);

                imagePos = ImGui::GetCursorStartPos();
                if(scaleHeight < 1) {
                    imageSize.x = size.x;
                    imageSize.y = size.y * scaleHeight;
                    imagePos.y += size.y * 0.5f - imageSize.y * 0.5f;
                }
                else {
                    imageSize.x = size.x / scaleHeight;
                    imageSize.y = size.y;
                    imagePos.x += size.x * 0.5f - imageSize.x * 0.5f;
                }

                previewRenderer.setDimensions(static_cast<uint32>(imageSize.x), static_cast<uint32>(imageSize.y));
            }

            previewRenderer.render();
            ImGui::SetCursorPos(imagePos);
            ImGui::Image((ImTextureID) (void*) previewRenderer.getRenderedImageId(), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    ImGui::End();
}
