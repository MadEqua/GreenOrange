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
                Object &obj = dynamic_cast<Object&>(*sceneEntity);
                ImGui::Text("Object Type:");
                ImGui::SameLine();
                ImGui::Text(ObjectTypeStrings[static_cast<int>(obj.getType())]);
                ImGui::NewLine();
                
                switch(obj.getType()) {
                case ObjectType::Sphere:
                {
                    Sphere &sphere = dynamic_cast<Sphere&>(obj);
                    float radius = sphere.getRadius();
                    ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, std::numeric_limits<float>::max());
                    sphere.setRadius(radius);
                    break;
                }

                case ObjectType::Box:
                {
                    Box &box = dynamic_cast<Box&>(obj);
                    float *dims = box.getDimensions();
                    ImGui::DragFloat3("Size", dims, 0.01f, 0.0f, std::numeric_limits<float>::max());
                    box.setDimensions(dims);
                    break;
                }

                default:
                    break;
                }
            }
        }
    }
    ImGui::End();
}