#include "ScenePanel.h"

#include <imgui.h>

#include "../model/Scene.h"
#include "../model/Object.h"
#include "../model/CsgOperator.h"
#include "imgui/ImGuiUtils.h"


void ScenePanel::drawGui(Scene &scene) {
    //ImGui::SetNextWindowPos(ImVec2(0, 200));
    ImGui::Begin("Scene", 0, 0);
    {
        if(scene.isEmpty()) {
            ImGui::Text("Empty Scene. Add an operator or an object.");
        }
        else if(scene.hasOperator()) {
            doOperatorNode(*scene.getOperator());
        }
        else {
            Object *obj = scene.getObject();
            if(ImGui::Selectable(obj->getName().c_str())) {
                //TODO
            }
        }
    }
    ImGui::End();
}

void ScenePanel::doOperatorNode(CsgOperator &op) const {
    if(ImGui::TreeNode(op.getName().c_str())) {
        doOperatorContextMenu(op);
        if(op.hasOperators()) {
            for(uint32 i = 0; i < op.getOperatorCount(); ++i) {
                doOperatorNode(op.getOperator(i));
            }
        }
        if(op.hasObjects()) {
            for(uint32 i = 0; i < op.getObjectCount(); ++i) {
                const Object &obj = op.getObject(i);
                if(ImGui::Selectable(obj.getName().c_str())) {
                    //TODO: click on object
                }
            }
        }
        ImGui::TreePop();
    }
}

void ScenePanel::doOperatorContextMenu(CsgOperator &op) const {
    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::BeginMenu("New CSG Operator")) {
            for(uint32 i = 0; i < CSG_OPERATOR_LIST_SIZE; ++i) {
                if(ImGui::MenuItem(CSG_OPERATOR_LIST[i].name.c_str())) {
                    op.createOperator("New operator", CSG_OPERATOR_LIST[i]);
                }
            }
            ImGui::EndMenu();
        }
        if(ImGui::Selectable("Rename")) {
            strcpy_s(buffer, op.getName().c_str());
            ImGui::OpenPopup("Rename Operator");
        }
        if(ImGui::Selectable("Delete")) {
            ImGui::OpenPopup("Delete Operator");
        }
        ImGui::EndPopup();
    }
    
    //TODO: not working
    if(ImGuiUtils::InputTextPopup("Rename Operator", "Enter a new name for the operator.", buffer, STRING_MAX_SIZE)) {
        op.setName(buffer);
        buffer[0] = 0;
    }
    if(ImGuiUtils::YesNoPopup("Delete Operator", "Delete the operator?\nThis operation cannot be undone!")) {
        //TODO
    }
}