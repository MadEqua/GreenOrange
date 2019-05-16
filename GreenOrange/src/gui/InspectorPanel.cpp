#include "InspectorPanel.h"

#include <limits>
#include <imgui.h>
#include "../model/GreenOrange.h"
#include "../model/CsgOperator.h"
#include "../model/Object.h"
#include "../DataRepo.h"


bool InspectorPanel::internalDrawGui(const GreenOrange &greenOrange) {
    SceneEntity *selectedEntity = greenOrange.getOpenProject()->getSelectedScene().getSelectedEntity();
    bool open;
    
    ImGui::Begin("Inspector", &open);
    {
        if(selectedEntity != nullptr) {
            ImGui::Text(selectedEntity->getName().c_str());

            ImGui::Text(SceneEntityTypeStrings[static_cast<int>(selectedEntity->getType())]);
            ImGui::SameLine();
            ImGui::Text("Type: ");
            ImGui::SameLine();

            if(selectedEntity->isCsgOperator()) {
                const CsgOperator &op = static_cast<const CsgOperator&>(*selectedEntity);
                ImGui::Text(CsgTypeStrings[static_cast<int>(op.getType())]);
                ImGui::NewLine();
            }
            else if(selectedEntity->isObject()) {
                Object &obj = static_cast<Object&>(*selectedEntity);
                ImGui::Text(ObjectTypeStrings[static_cast<int>(obj.getType())]);
                ImGui::NewLine();
                
                switch(obj.getType()) {
                case ObjectType::Sphere:
                {
                    Sphere &sphere = static_cast<Sphere&>(obj);
                    float radius = sphere.getRadius();
                    if(ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, std::numeric_limits<float>::max()))
                        sphere.setRadius(radius);
                    break;
                }

                case ObjectType::Box:
                {
                    Box &box = static_cast<Box&>(obj);
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