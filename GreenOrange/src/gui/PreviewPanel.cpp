#include "PreviewPanel.h"

#include "../GlslGenerator.h"
#include "../model/Project.h"
#include "../Constants.h"


PreviewPanel::PreviewPanel() :
    previousSize(-1.0f, -1.0f) {
}

//TODO project should be const
void PreviewPanel::drawGui(Project &project) {
    ImGui::Begin("Preview");
    {
        //TODO for testing, delete
        bool hasChanges = false;
        if(ImGui::Button("GENERATE CODE!!!1!!")) {
            hasChanges = true;
        }

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

                previewRenderer.setDimensions(imageSize.x, imageSize.y);
            }

            if(hasChanges) {
                GlslGenerator generator;
                previewRenderer.setFragmentShader(generator.generate(project).c_str());
                previewRenderer.setDimensions(imageSize.x, imageSize.y);
                hasChanges = false;
            }

            previewRenderer.render();
            ImGui::SetCursorPos(imagePos);
            ImGui::Image((ImTextureID) (void*) previewRenderer.getRenderedImageId(), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    ImGui::End();
}
