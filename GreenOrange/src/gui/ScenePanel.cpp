#include "ScenePanel.h"

#include <imgui.h>

#include "../model/Scene.h"
#include "../model/Object.h"
#include "../model/Operator.h"

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

void ScenePanel::doOperatorNode(Operator &op) const {
    if(ImGui::TreeNode(op.getName().c_str())) {
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