#include "ScenePanel.h"

#include <imgui.h>

#include "../model/GreenOrange.h"
#include "../model/Scene.h"
#include "../model/Object.h"
#include "../model/CsgOperator.h"
#include "../DataRepo.h"
#include "../Constants.h"
#include "../Assert.h"
#include "imgui/ImGuiUtils.h"


bool ScenePanel::internalDrawGui(const GreenOrange &greenOrange) {
    Scene &scene = greenOrange.getOpenProject()->getSelectedScene();
    bool open;
    
    ImGui::Begin("Scene", &open);
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
        if(ImGui::CollapsingHeader("CSG Tree", flags)) {
            doOperatorNode(scene, scene.getCsgTreeRootNode());
        }

        ImGui::NewLine();
        ImGui::NewLine();

        if(ImGui::CollapsingHeader("Transform Trees", flags)) {
            for(uint32 i = 0; i < scene.getTransformTreeCount(); ++i)
                doTransformNode(scene, i, scene.getTransformTreeRootNodeByIndex(i));

            ImGui::NewLine();
            if(ImGui::Button("New Tree")) {
                ImGui::OpenPopup("newTreePopup");
            }

            if(ImGui::BeginPopupContextItem("newTreePopup")) {
                for(uint32 i = 0; i < static_cast<int>(TransformType::Size); ++i) {
                    if(ImGui::MenuItem(TransformTypeStrings[i])) {
                        scene.createRootTransform(TransformTypeStrings[i], static_cast<TransformType>(i));
                    }
                }
                ImGui::EndPopup();
            }
        }
    }
    ImGui::End();

    return open;
}

void ScenePanel::doOperatorNode(Scene &scene, TreeNode<SceneEntity> &node) const {
    CsgOperator &csgOp = static_cast<CsgOperator&>(*node);
    
    uint32 id = csgOp.getId();
    ImGui::PushID(id);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
    if(!node.hasChildren()) flags |= ImGuiTreeNodeFlags_Leaf;

    SceneEntity *selectedEntity = scene.getSelectedEntity();
    if(selectedEntity && csgOp == *selectedEntity) flags |= ImGuiTreeNodeFlags_Selected;

    bool treeNodeOpen = ImGui::TreeNodeExV(&id, flags, csgOp.getName().c_str(), "");
    if(ImGui::IsItemClicked()) 
        scene.setSelectedEntity(csgOp);

    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        DndPayload payload = {DndPayload::DndType::Csg, 0, &node};
        ImGui::SetDragDropPayload(DND_PAYLOAD, &payload, sizeof(DndPayload));
        ImGui::Text(csgOp.getName().c_str());
        ImGui::EndDragDropSource();
    }
    if(ImGui::BeginDragDropTarget()) {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DND_PAYLOAD)) {
            GO_ASSERT(payload->DataSize == sizeof(DndPayload));
            DndPayload *dndPayload = static_cast<DndPayload*>(payload->Data);
            if(dndPayload->type == DndPayload::DndType::Csg || dndPayload->type == DndPayload::DndType::Object)
                scene.moveCsgTreeNode(*static_cast<TreeNode<SceneEntity>*>(dndPayload->dataPtr), node);
        }
        ImGui::EndDragDropTarget();
    }

    doOperatorContextMenu(scene, node);

    if(treeNodeOpen) {
        for(uint32 i = 0; i < node.getChildCount(); ++i) {
            TreeNode<SceneEntity> &childNode = node.getChildByIndex(i);
            if(childNode->isCsgOperator())
                doOperatorNode(scene, childNode);
            else if(childNode->isObject())
                doObjectNode(scene, childNode);
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void ScenePanel::doObjectNode(Scene &scene, TreeNode<SceneEntity> &node) const {
    Object &obj = static_cast<Object&>(*node);

    uint32 id = obj.getId();
    ImGui::PushID(id);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;

    SceneEntity *selectedEntity = scene.getSelectedEntity();
    if(selectedEntity && obj == *selectedEntity) flags |= ImGuiTreeNodeFlags_Selected;

    ImGui::TreeNodeExV(&id, flags, obj.getName().c_str(), "");
    if(ImGui::IsItemClicked()) scene.setSelectedEntity(obj);

    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        DndPayload payload = {DndPayload::DndType::Object, 0, &node};
        ImGui::SetDragDropPayload(DND_PAYLOAD, &payload, sizeof(DndPayload));
        ImGui::Text(obj.getName().c_str());
        ImGui::EndDragDropSource();
    }

    doObjectContextMenu(scene, node);

    ImGui::TreePop();
    ImGui::PopID();
}

void ScenePanel::doObjectContextMenu(Scene &scene, TreeNode<SceneEntity> &node) const {
    Object &obj = static_cast<Object&>(*node);

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
        scene.deleteCsgTreeNode(node);
    }
}

void ScenePanel::doOperatorContextMenu(Scene &scene, TreeNode<SceneEntity> &node) const {
    CsgOperator &csgOp = static_cast<CsgOperator&>(*node);

    bool openRenamePopup = false;
    bool openDeletePopup = false;
    bool openDeleteChildrenPopup = false;

    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::BeginMenu("New CSG Operator")) {
            for(uint32 i = 0; i < static_cast<int>(CsgType::Size); ++i) {
                if(ImGui::MenuItem(CsgTypeStrings[i])) {
                    scene.createCsgOperator(CsgTypeStrings[i], static_cast<CsgType>(i), node);
                }
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("New Object")) {
            for(uint32 i = 0; i < static_cast<int>(ObjectType::Size); ++i) {
                if(ImGui::MenuItem(ObjectTypeStrings[i])) {
                    scene.createObject(ObjectTypeStrings[i], static_cast<ObjectType>(i), node);
                }
            }
            ImGui::EndMenu();
        }
        if(ImGui::Selectable("Rename")) {
            openRenamePopup = true;
        }
        if(scene.getCsgRootOperator() != csgOp && ImGui::Selectable("Delete")) {
            openDeletePopup = true;
        }
        if(node.hasChildren() && ImGui::Selectable("Delete Children")) {
            openDeleteChildrenPopup = true;
        }

        ImGui::EndPopup();
    }
    
    if(openRenamePopup) {
        strcpy_s(inputBuffer, csgOp.getName().c_str());
        ImGui::OpenPopup("Rename Operator");
    }
    const char *newNameString = "Enter a new name for the operator %s.";
    sprintf_s(stringBuffer, newNameString, csgOp.getName().c_str());
    if(ImGuiUtils::InputTextPopup("Rename Operator", stringBuffer, inputBuffer, INPUT_STRING_MAX_SIZE)) {
        csgOp.setName(inputBuffer);
    }

    if(openDeletePopup) {
        ImGui::OpenPopup("Delete Operator");
    }

    if(openDeleteChildrenPopup) {
        ImGui::OpenPopup("Delete Operator Children");
    }
    const char *clearString = "Delete the operator %s children?\nThis operation cannot be undone!";
    sprintf_s(stringBuffer, clearString, csgOp.getName().c_str());
    if(ImGuiUtils::YesNoPopup("Delete Operator Children", stringBuffer)) {
        scene.deleteCsgTreeNodeChildren(node);
    }

    const char *deleteString = "Delete the operator %s?\nThis operation cannot be undone!";
    sprintf_s(stringBuffer, deleteString, csgOp.getName().c_str());
    if(ImGuiUtils::YesNoPopup("Delete Operator", stringBuffer)) {
        scene.deleteCsgTreeNode(node);
    }
}

void ScenePanel::doTransformNode(Scene &scene, uint32 treeIndex, TreeNode<Transform> &node) const {
    Transform &transform = static_cast<Transform&>(*node);

    uint32 id = transform.getId();
    ImGui::PushID(id);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
    if(!node.hasChildren()) flags |= ImGuiTreeNodeFlags_Leaf;

    SceneEntity *selectedEntity = scene.getSelectedEntity();
    if(selectedEntity && transform == *selectedEntity) flags |= ImGuiTreeNodeFlags_Selected;

    bool treeNodeOpen = ImGui::TreeNodeExV(&id, flags, transform.getName().c_str(), "");
    if(ImGui::IsItemClicked())
        scene.setSelectedEntity(transform);

    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        DndPayload payload = {DndPayload::DndType::Transform, treeIndex, &node};
        ImGui::SetDragDropPayload(DND_PAYLOAD, &payload, sizeof(DndPayload));
        ImGui::Text(transform.getName().c_str());
        ImGui::EndDragDropSource();
    }
    if(ImGui::BeginDragDropTarget()) {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DND_PAYLOAD)) {
            GO_ASSERT(payload->DataSize == sizeof(DndPayload));
            DndPayload *dndPayload = static_cast<DndPayload*>(payload->Data);
            if(dndPayload->type == DndPayload::DndType::Transform)
                scene.moveTransformTreeNode(dndPayload->intData, *static_cast<TreeNode<Transform>*>(dndPayload->dataPtr), node);
            else if(dndPayload->type == DndPayload::DndType::Object)
                scene.attachObjectToTransformTreeNode(static_cast<Object&>(static_cast<TreeNode<SceneEntity>*>(dndPayload->dataPtr)->getPayload()), node);
            else if(dndPayload->type == DndPayload::DndType::TransformAtttachment)
                scene.attachObjectToTransformTreeNode(*static_cast<Object*>(dndPayload->dataPtr), node);
        }
        ImGui::EndDragDropTarget();
    }

    doTransformContextMenu(scene, treeIndex, node);

    if(treeNodeOpen) {
        for(uint32 i = 0; i < node.getChildCount(); ++i) {
            TreeNode<Transform> &childNode = node.getChildByIndex(i);
            doTransformNode(scene, treeIndex, childNode);
        }
        doTransformAttachments(scene, treeIndex, node);
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void ScenePanel::doTransformAttachments(Scene &scene, uint32 treeIndex, TreeNode<Transform> &node) const {
    scene.getCsgTreeRootNode().traverseBfs([&scene, treeIndex, &node, this](TreeNode<SceneEntity> &entityNode, TreeNode<SceneEntity> *parent) -> bool {
        if(entityNode->isObject()) {
            Object &obj = static_cast<Object&>(*entityNode);
            if(obj.isAttachedToTransform() && obj.getTransformId() == node->getId()) {
                uint32 id = obj.getId();

                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;

                SceneEntity *selectedEntity = scene.getSelectedEntity();
                if(selectedEntity && obj == *selectedEntity) flags |= ImGuiTreeNodeFlags_Selected;

                ImGui::PushID(obj.getId());
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                if(ImGui::TreeNodeExV(&id, flags, obj.getName().c_str(), "")) {
                    if(ImGui::IsItemClicked())
                        scene.setSelectedEntity(obj);

                    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        DndPayload payload = {DndPayload::DndType::TransformAtttachment, treeIndex, &obj};
                        ImGui::SetDragDropPayload(DND_PAYLOAD, &payload, sizeof(DndPayload));
                        ImGui::Text(obj.getName().c_str());
                        ImGui::EndDragDropSource();
                    }

                    ImGui::PopStyleColor();
                    if(ImGui::BeginPopupContextItem()) {
                        if(ImGui::Selectable("Detach")) {
                            scene.detachObjectToTransformTreeNode(obj);
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        }
        return false;
    });
}


void ScenePanel::doTransformContextMenu(Scene &scene, uint32 treeIndex, TreeNode<Transform> &node) const {
    Transform &transform = static_cast<Transform&>(*node);

    bool openRenamePopup = false;
    bool openDeletePopup = false;
    bool openDeleteChildrenPopup = false;

    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::BeginMenu("New Transform")) {
            for(uint32 i = 0; i < static_cast<int>(TransformType::Size); ++i) {
                if(ImGui::MenuItem(TransformTypeStrings[i])) {
                    scene.createTransform(TransformTypeStrings[i], static_cast<TransformType>(i), node);
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
        if(node.hasChildren() && ImGui::Selectable("Delete Children")) {
            openDeleteChildrenPopup = true;
        }

        ImGui::EndPopup();
    }

    if(openRenamePopup) {
        strcpy_s(inputBuffer, transform.getName().c_str());
        ImGui::OpenPopup("Rename Transform");
    }
    const char *newNameString = "Enter a new name for the transform %s.";
    sprintf_s(stringBuffer, newNameString, transform.getName().c_str());
    if(ImGuiUtils::InputTextPopup("Rename Transform", stringBuffer, inputBuffer, INPUT_STRING_MAX_SIZE)) {
        transform.setName(inputBuffer);
    }

    if(openDeletePopup) {
        ImGui::OpenPopup("Delete Transform");
    }

    if(openDeleteChildrenPopup) {
        ImGui::OpenPopup("Delete Transform Children");
    }
    const char *clearString = "Delete the transform %s children?\nThis operation cannot be undone!";
    sprintf_s(stringBuffer, clearString, transform.getName().c_str());
    if(ImGuiUtils::YesNoPopup("Delete Transform Children", stringBuffer)) {
        scene.deleteTransformTreeNodeChildren(treeIndex, node);
    }

    const char *deleteString = "Delete the transform %s?\nThis operation cannot be undone!";
    sprintf_s(stringBuffer, deleteString, transform.getName().c_str());
    if(ImGuiUtils::YesNoPopup("Delete Transform", stringBuffer)) {
        /*if(*scene.getTransformTreeRootNodeByIndex(treeIndex) == transform) {
            scene.deleteRootTransform(treeIndex);
        }
        else {
            scene.deleteTransformTreeNode(treeIndex, node);
        }*/

        scene.deleteTransformTreeNode(treeIndex, node);

    }
}