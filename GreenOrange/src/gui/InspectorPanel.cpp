#include "InspectorPanel.h"

#include <imgui.h>

#include "../model/GreenOrange.h"
#include "../model/CsgOperator.h"
#include "../model/Object.h"
#include "../model/Transform.h"
#include "../DataRepo.h"
#include "../Constants.h"


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
                    if(ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, DRAG_MAX))
                        sphere.setRadius(radius);
                    break;
                }

                case ObjectType::Box:
                {
                    Box &box = static_cast<Box&>(obj);
                    float *dims = box.getDimensions();
                    if(ImGui::DragFloat3("Size", dims, 0.01f, 0.0f, DRAG_MAX))
                        box.setDimensions(dims);
                    break;
                }

                default:
                    break;
                }
            }
            else if(selectedEntity->isTransform()) {
                Transform &tr = static_cast<Transform&>(*selectedEntity);
                ImGui::Text(TransformTypeStrings[static_cast<int>(tr.getType())]);
                ImGui::NewLine();

                switch(tr.getType()) {
                case TransformType::Translation:
                {
                    Translation &trans = static_cast<Translation&>(tr);
                    float *ammount = trans.getAmmount();
                    if(ImGui::DragFloat3("Ammount", ammount, 0.01f, DRAG_MIN, DRAG_MAX))
                        trans.setAmmount(ammount);
                    break;
                }
                case TransformType::Rotation:
                {
                    Rotation &rot = static_cast<Rotation&>(tr);
                    float *ammount = rot.getAmmount();
                    if(ImGui::DragFloat3("Ammount", ammount, 0.01f, DRAG_MIN, DRAG_MAX))
                        rot.setAmmount(ammount);
                    break;
                }
                case TransformType::Custom:
                {
                    //TODO: make this better
                    CustomTransform &custom = static_cast<CustomTransform&>(tr);
                    //strcpy_s(inputBuffer, custom.getCode().c_str());
                    ImGui::InputText("Code", inputBuffer, INPUT_STRING_MAX_SIZE);
                    if(ImGui::Button("Save")) {
                        custom.setCode(inputBuffer);
                    }
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