#include "ScenePanel.h"

#include <imgui.h>

#include "../model/Scene.h"
#include "../model/Object.h"
#include "../model/CsgOperator.h"
#include "../model/DataRepo.h"
#include "imgui/ImGuiUtils.h"


void ScenePanel::drawGui(Scene &scene) {
    //ImGui::SetNextWindowPos(ImVec2(0, 200));
    ImGui::Begin("Scene", 0, 0);
    {
        doOperatorNode(scene, scene.getRootOperator());
    }
    ImGui::End();
}

void ScenePanel::doOperatorNode(Scene &scene, CsgOperator &op) const {
    uint32 id = op.getId();
    bool treeNodeOpen = ImGui::TreeNode(&id, "%s", op.getName().c_str());
    doOperatorContextMenu(scene, op);

    if(treeNodeOpen) {
        if(op.hasOperators()) {
            for(uint32 i = 0; i < op.getOperatorCount(); ++i) {
                doOperatorNode(scene, op.getOperatorByIndex(i));
            }
        }
        if(op.hasObjects()) {
            for(uint32 i = 0; i < op.getObjectCount(); ++i) {
                const Object &obj = op.getObjectByIndex(i);
                if(ImGui::Selectable(obj.getName().c_str())) {
                    //TODO: click on object
                }
            }
        }
        ImGui::TreePop();
    }
}

void ScenePanel::doOperatorContextMenu(Scene &scene, CsgOperator &op) const {
    bool openRenamePopup = false;
    bool openDeletePopup = false;

    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::BeginMenu("New CSG Operator")) {
            for(uint32 i = 0; i < static_cast<int>(CsgType::Size); ++i) {
                if(ImGui::MenuItem(CsgTypeStrings[i])) {
                    scene.createCsgOperator(CsgTypeStrings[i], static_cast<CsgType>(i), op);
                }
            }
            ImGui::EndMenu();
        }
        if(ImGui::Selectable("Rename")) {
            strcpy_s(buffer, op.getName().c_str());
            openRenamePopup = true;
        }
        if(ImGui::Selectable("Delete")) {
            openDeletePopup = true;
        }

        ImGui::EndPopup();
    }
    
    if(openRenamePopup) {
        ImGui::OpenPopup("Rename Operator");
    }
    if(ImGuiUtils::InputTextPopup("Rename Operator", "Enter a new name for the operator.", buffer, STRING_MAX_SIZE)) {
        op.setName(buffer);
        buffer[0] = 0;
    }

    if(openDeletePopup) {
        ImGui::OpenPopup("Delete Operator");
    }
    if(ImGuiUtils::YesNoPopup("Delete Operator", "Delete the operator?\nThis operation cannot be undone!")) {
        scene.deleteCsgOperator(op);
    }
}