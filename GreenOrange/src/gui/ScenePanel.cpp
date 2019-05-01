#include "ScenePanel.h"

#include <imgui.h>

void ScenePanel::drawGui() {
    ImGui::SetNextWindowPos(ImVec2(0, 200));
    ImGui::Begin("Scene Objects", 0, ImGuiWindowFlags_AlwaysAutoResize);
    {
        if(ImGui::TreeNode("Basic trees")) {
            for(int i = 0; i < 5; i++) {
                if(ImGui::TreeNode((void*) (intptr_t) i, "Child %d", i)) {
                    if(ImGui::Selectable("blah blah")) {
                        //TODO
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}