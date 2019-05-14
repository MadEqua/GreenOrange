#include "InspectorPanel.h"

#include <limits>
#include <imgui.h>
#include "../model/GreenOrange.h"
#include "../model/CsgOperator.h"
#include "../model/Object.h"
#include "../DataRepo.h"


bool InspectorPanel::internalDrawGui(const GreenOrange &greenOrange) {
    SceneEntity *sceneEntity = greenOrange.getOpenProject()->getSelectedScene().getSelectedEntity();
    bool open;
    
    ImGui::Begin("Inspector", &open);
    {
        if(sceneEntity != nullptr) {
            ImGui::Text(sceneEntity->getName().c_str());

            ImGui::Text(SceneEntityTypeStrings[static_cast<int>(sceneEntity->getType())]);
            ImGui::SameLine();
            ImGui::Text("Type: ");
            ImGui::SameLine();

            if(sceneEntity->isCsgOperator()) {
                const CsgOperator &op = dynamic_cast<const CsgOperator&>(*sceneEntity);
                ImGui::Text(CsgTypeStrings[static_cast<int>(op.getType())]);
                ImGui::NewLine();
            }
            else if(sceneEntity->isObject()) {
                Object &obj = dynamic_cast<Object&>(*sceneEntity);
                ImGui::Text(ObjectTypeStrings[static_cast<int>(obj.getType())]);
                ImGui::NewLine();
                
                switch(obj.getType()) {
                case ObjectType::Sphere:
                {
                    Sphere &sphere = dynamic_cast<Sphere&>(obj);
                    float radius = sphere.getRadius();
                    if(ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, std::numeric_limits<float>::max()))
                        sphere.setRadius(radius);
                    break;
                }

                case ObjectType::Box:
                {
                    Box &box = dynamic_cast<Box&>(obj);
                    float *dims = box.getDimensions();
                    if(ImGui::DragFloat3("Size", dims, 0.01f, 0.0f, std::numeric_limits<float>::max()))
                        box.setDimensions(dims);
                    break;
                }

                default:
                    break;
                }
            }
        }
        else {
            ImGui::Text("Nothing selected");
        }
    }
    ImGui::End();

    return open;
}