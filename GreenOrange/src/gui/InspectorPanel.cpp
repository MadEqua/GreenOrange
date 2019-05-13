#include "InspectorPanel.h"

#include <limits>
#include <imgui.h>
#include "../model/CsgOperator.h"
#include "../model/Object.h"
#include "../model/DataRepo.h"


void InspectorPanel::drawGui(SceneEntity *sceneEntity) const {
    static float vec[4] = {0.10f, 0.20f, 0.30f, 0.44f};
    ImGui::Begin("Inspector", 0, 0);
    {
        //ImGui::DragFloat3("World Position", vec, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
        //ImGui::DragFloat3("Orientation", vec, 0.01f, 0.0f, 359.0f);
        if(sceneEntity != nullptr) {
            ImGui::Text(sceneEntity->getName().c_str());

            if(sceneEntity->isCsgOperator()) {
                const CsgOperator &op = dynamic_cast<const CsgOperator&>(*sceneEntity);
                ImGui::Text("CSG Operator Type:");
                ImGui::SameLine();
                ImGui::Text(CsgTypeStrings[static_cast<int>(op.getType())]);
                ImGui::NewLine();
            }
            else if(sceneEntity->isObject()) {
                const Object &obj = dynamic_cast<const Object&>(*sceneEntity);
                ImGui::Text("Object Type:");
                ImGui::SameLine();
                ImGui::Text(ObjectTypeStrings[static_cast<int>(obj.getType())]);
                ImGui::NewLine();
                
                switch(obj.getType()) {
                case ObjectType::Sphere:
                    ImGui::DragFloat("Radius", vec, 0.01f, 0.0f, std::numeric_limits<float>::max());
                    break;

                case ObjectType::Box:
                    ImGui::DragFloat3("Size", vec, 0.01f, 0.0f, std::numeric_limits<float>::max());
                    break;

                default:
                    break;
                }
            }
        }
    }
    ImGui::End();
}