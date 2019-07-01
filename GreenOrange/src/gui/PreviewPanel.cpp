#include "PreviewPanel.h"

#include "../model/GreenOrange.h"
#include "../Constants.h"
#include "../GlslGenerator.h"
#include "GuiRoot.h"


PreviewPanel::PreviewPanel(GuiRoot &guiRoot) :
    Panel(guiRoot, PanelType::Preview, true),
    previousSize(-1.0f, -1.0f) {
}

bool PreviewPanel::internalDrawGui(const GreenOrange &greenOrange) {
    bool open;

    ImGui::Begin("Preview", &open);
    {
        bool onlyPreviewStatic = greenOrange.getOpenProject()->getOnlyPreviewStaticObjects();
        ImGui::Checkbox("Show Only Static Objects", &onlyPreviewStatic);
        greenOrange.getOpenProject()->setOnlyPreviewStaticObjects(onlyPreviewStatic);

        ImGui::SameLine();

        bool previewProbes = greenOrange.getOpenProject()->getPreviewProbes();
        ImGui::Checkbox("Show Probes", &previewProbes);
        greenOrange.getOpenProject()->setPreviewProbes(previewProbes);

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

            //Only change shader if code has changed since last Panel update
            uint64 currentCodeId = GlslGenerator::getInstance().getCurrentCodeId();
            if(lastCodeId != currentCodeId) {
                lastCodeId = currentCodeId;
                previewRenderer.setFragmentShader(GlslGenerator::getInstance().getGlslCode().c_str());
                previewRenderer.setDimensions(imageSize.x, imageSize.y);
            }

            previewRenderer.render();
            ImGui::SetCursorPos(imagePos);
            ImGui::Image((ImTextureID) (void*) previewRenderer.getRenderedImageId(), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    ImGui::End();

    return open;
}
