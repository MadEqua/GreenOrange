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
        doOperatorNode(scene, scene.getRootCsgOperator());
    }
    ImGui::End();
}

void ScenePanel::doOperatorNode(Scene &scene, CsgOperator &op) const {
    constexpr const char *DND_PAYLOAD = "DND_PAYLOAD";

    uint32 id = op.getId();
    ImGui::PushID(id);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    if(op.isEmpty()) flags |= ImGuiTreeNodeFlags_Leaf;
    bool treeNodeOpen = ImGui::TreeNodeExV(&id, flags, op.getName().c_str(), "");

    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        CsgOperator* ptr = &op;
        ImGui::SetDragDropPayload(DND_PAYLOAD, &ptr, sizeof(CsgOperator*));
        ImGui::Text(op.getName().c_str());
        ImGui::EndDragDropSource();
    }
    if(ImGui::BeginDragDropTarget()) {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DND_PAYLOAD)) {
            GO_ASSERT(payload->DataSize == sizeof(CsgOperator*));
            CsgOperator **castedPayload = static_cast<CsgOperator**>(payload->Data);
            scene.moveCsgOperator(**castedPayload, op);
        }
        ImGui::EndDragDropTarget();
    }

    doOperatorContextMenu(scene, op);

    if(treeNodeOpen) {
        for(uint32 i = 0; i < op.getChildOperatorCount(); ++i) {
            doOperatorNode(scene, op.getChildOperatorByIndex(i));
        }
        for(uint32 i = 0; i < op.getChildObjectCount(); ++i) {
            if(ImGui::Selectable(op.getChildObjectByIndex(i).getName().c_str())) {
                //TODO: click on object
            }
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
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
            openRenamePopup = true;
        }
        if(ImGui::Selectable("Delete")) {
            openDeletePopup = true;
        }

        ImGui::EndPopup();
    }
    
    if(openRenamePopup) {
        strcpy_s(inputBuffer, op.getName().c_str());
        ImGui::OpenPopup("Rename Operator");
    }
    const char *newNameString = "Enter a new name for the operator %s.";
    sprintf_s(stringBuffer, newNameString, op.getName().c_str());
    if(ImGuiUtils::InputTextPopup("Rename Operator", stringBuffer, inputBuffer, INPUT_STRING_MAX_SIZE)) {
        op.setName(inputBuffer);
    }

    if(openDeletePopup) {
        ImGui::OpenPopup("Delete Operator");
    }
    const char *deleteString = "Delete the operator %s?\nThis operation cannot be undone!";
    sprintf_s(stringBuffer, deleteString, op.getName().c_str());
    if(ImGuiUtils::YesNoPopup("Delete Operator", stringBuffer)) {
        scene.deleteCsgOperator(op);
    }
}