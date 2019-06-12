#include "InspectorPanel.h"

#include <imgui.h>

#include "../model/GreenOrange.h"
#include "../model/CsgOperator.h"
#include "../model/Object.h"
#include "../model/Transform.h"
#include "../model/Light.h"
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
                doCsgOperator(static_cast<CsgOperator&>(*selectedEntity));
            }
            else if(selectedEntity->isObject()) {
                doObject(static_cast<Object&>(*selectedEntity));
            }
            else if(selectedEntity->isTransform()) {
                doTransform(static_cast<Transform&>(*selectedEntity));
            }
            else if(selectedEntity->isLight()) {
                doLight(static_cast<Light&>(*selectedEntity));
            }
        }
        else {
            ImGui::Text("Nothing selected");
        }
    }
    ImGui::End();

    return open;
}

void InspectorPanel::doCsgOperator(CsgOperator &csgOperator) {
    ImGui::Text(CsgTypeStrings[static_cast<int>(csgOperator.getType())]);
    ImGui::NewLine();
}

void InspectorPanel::doObject(Object &object) {
    ImGui::Text(ObjectTypeStrings[static_cast<int>(object.getType())]);
    ImGui::NewLine();

    switch(object.getType()) {
    case ObjectType::Sphere:
    {
        Sphere &sphere = static_cast<Sphere&>(object);
        float radius = sphere.getRadius();
        if(ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, DRAG_MAX))
            sphere.setRadius(radius);
        break;
    }
    case ObjectType::Box:
    {
        Box &box = static_cast<Box&>(object);
        float *dims = box.getDimensions();
        if(ImGui::DragFloat3("Size", dims, 0.01f, 0.0f, DRAG_MAX))
            box.setDimensions(dims);
        break;
    }
    default:
        GO_ASSERT_ALWAYS();
        break;
    }
}

void InspectorPanel::doTransform(Transform &transform) {
    ImGui::Text(TransformTypeStrings[static_cast<int>(transform.getType())]);
    ImGui::NewLine();

    switch(transform.getType()) {
    case TransformType::Translation:
    {
        Translation &trans = static_cast<Translation&>(transform);
        float *ammount = trans.getAmmount();
        if(ImGui::DragFloat3("Ammount", ammount, 0.05f, DRAG_MIN, DRAG_MAX))
            trans.setAmmount(ammount);
        break;
    }
    case TransformType::Rotation:
    {
        Rotation &rot = static_cast<Rotation&>(transform);
        float *ammount = rot.getAmmount();
        if(ImGui::DragFloat3("Ammount", ammount, 1.0f, -180.0f, 180.0f))
            rot.setAmmount(ammount);
        break;
    }
    case TransformType::Custom:
    {
        //TODO: make this better
        CustomTransform &custom = static_cast<CustomTransform&>(transform);
        //strcpy_s(inputBuffer, custom.getCode().c_str());
        ImGui::InputText("Code", inputBuffer, INPUT_STRING_MAX_SIZE);
        if(ImGui::Button("Save")) {
            custom.setCode(inputBuffer);
        }
        break;
    }
    default:
        GO_ASSERT_ALWAYS();
        break;
    }
}

void InspectorPanel::doLight(Light &light) {
    ImGui::Text(LightTypeStrings[static_cast<int>(light.getType())]);
    ImGui::NewLine();

    switch(light.getType()) {
    case LightType::Directional:
    {
        DirectionalLight &dirLight = static_cast<DirectionalLight&>(light);
        float *dir = dirLight.getDirection();
        if(ImGui::DragFloat3("Direction", dir, 0.01f, -1.0f, 1.0f))
            dirLight.setDirection(dir);
        break;
    }
    case LightType::Point:
    {
        PointLight &pointLight = static_cast<PointLight&>(light);
        float *pos = pointLight.getPosition();
        if(ImGui::DragFloat3("Position", pos, 0.05f, DRAG_MIN, DRAG_MAX))
            pointLight.setPosition(pos);
        break;
    }
    default:
        GO_ASSERT_ALWAYS();
        break;
    }

    ImGui::NewLine();

    ImGuiColorEditFlags colorPickerFlags = 0;// ImGuiColorEditFlags_NoInputs;
    float *color = light.getColor();
    if(ImGui::ColorPicker3("Color", color, colorPickerFlags))
        light.setColor(color);
}