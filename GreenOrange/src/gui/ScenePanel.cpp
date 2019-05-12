#include "ScenePanel.h"

#include <imgui.h>

#include "../model/Scene.h"
#include "../model/Object.h"
#include "../model/CsgOperator.h"
#include "../model/DataRepo.h"
#include "../Constants.h"
#include "imgui/ImGuiUtils.h"


void ScenePanel::drawGui(Scene &scene) const {
    ImGui::Begin("Scene", 0, 0);
    {
        doOperatorNode(scene, scene.getRootCsgOperator());
    }
    ImGui::End();
}

void ScenePanel::doOperatorNode(Scene &scene, CsgOperator &op) const {
    uint32 id = op.getId();
    ImGui::PushID(id);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    if(op.isEmpty()) flags |= ImGuiTreeNodeFlags_Leaf;
    bool treeNodeOpen = ImGui::TreeNodeExV(&id, flags, op.getName().c_str(), "");

    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        CsgOperator* ptr = &op;
        DndPayload payload = {DND_CSG_OPERATOR, ptr};
        ImGui::SetDragDropPayload(DND_PAYLOAD, &payload, sizeof(DndPayload));
        ImGui::Text(op.getName().c_str());
        ImGui::EndDragDropSource();
    }
    if(ImGui::BeginDragDropTarget()) {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DND_PAYLOAD)) {
            GO_ASSERT(payload->DataSize == sizeof(DndPayload));
            DndPayload *dndPayload = static_cast<DndPayload*>(payload->Data);
            if(dndPayload->type == DND_OBJECT) {
                scene.moveObject(*static_cast<Object*>(dndPayload->dataPtr), op);
            }
            else if(dndPayload->type == DND_CSG_OPERATOR) {
                scene.moveCsgOperator(*static_cast<CsgOperator*>(dndPayload->dataPtr), op);
            }
        }
        ImGui::EndDragDropTarget();
    }

    doOperatorContextMenu(scene, op);

    if(treeNodeOpen) {
        for(uint32 i = 0; i < op.getChildOperatorCount(); ++i) {
            doOperatorNode(scene, op.getChildOperatorByIndex(i));
        }
        for(uint32 i = 0; i < op.getChildObjectCount(); ++i) {
            doObjectNode(scene, op.getChildObjectByIndex(i));
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void ScenePanel::doObjectNode(Scene &scene, Object &obj) const {
    uint32 id = obj.getId();
    ImGui::PushID(id);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;
    bool treeNodeOpen = ImGui::TreeNodeExV(&id, flags, obj.getName().c_str(), "");

    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        Object* ptr = &obj;
        DndPayload payload = {DND_OBJECT, ptr};
        ImGui::SetDragDropPayload(DND_PAYLOAD, &payload, sizeof(DndPayload));
        ImGui::Text(obj.getName().c_str());
        ImGui::EndDragDropSource();
    }

    doObjectContextMenu(scene, obj);

    ImGui::TreePop();
    ImGui::PopID();
}

void ScenePanel::doObjectContextMenu(Scene &scene, Object &obj) const {
    bool openRenamePopup = false;
    bool openDeletePopup = false;

    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::Selectable("Rename")) {
            openRenamePopup = true;
        }
        if(ImGui::Selectable("Delete")) {
            openDeletePopup = true;
        }

        ImGui::EndPopup();
    }

    if(openRenamePopup) {
        strcpy_s(inputBuffer, obj.getName().c_str());
        ImGui::OpenPopup("Rename Object");
    }
    const char *newNameString = "Enter a new name for the object %s.";
    sprintf_s(stringBuffer, newNameString, obj.getName().c_str());
    if(ImGuiUtils::InputTextPopup("Rename Object", stringBuffer, inputBuffer, INPUT_STRING_MAX_SIZE)) {
        obj.setName(inputBuffer);
    }

    if(openDeletePopup) {
        ImGui::OpenPopup("Delete Object");
    }
    const char *deleteString = "Delete the object %s?\nThis operation cannot be undone!";
    sprintf_s(stringBuffer, deleteString, obj.getName().c_str());
    if(ImGuiUtils::YesNoPopup("Delete Object", stringBuffer)) {
        scene.deleteObject(obj);
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
        if(ImGui::BeginMenu("New Object")) {
            for(uint32 i = 0; i < static_cast<int>(ObjectType::Size); ++i) {
                if(ImGui::MenuItem(ObjectTypeStrings[i])) {
                    scene.createObject(ObjectTypeStrings[i], static_cast<ObjectType>(i), op);
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