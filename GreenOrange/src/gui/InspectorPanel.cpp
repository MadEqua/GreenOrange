#include "InspectorPanel.h"

#include <imgui.h>

#include "../model/GreenOrange.h"
#include "../model/Project.h"
#include "../model/CsgOperator.h"
#include "../model/Object.h"
#include "../model/Transform.h"
#include "../model/Light.h"
#include "../model/Material.h"
#include "../DataRepo.h"
#include "../Constants.h"
#include "GuiRoot.h"

#include <glm/gtc/type_ptr.hpp>


static bool materialComboItemGetter(void* data, int idx, const char** out_str) {
    if(data != nullptr) {
        Project *project = static_cast<Project*>(data);
        if(idx < project->getMaterialCount()) {
            (*out_str) = project->getMaterialByIndex(idx).getName().c_str();
            return true;
        }
    }
    return false;
}

bool InspectorPanel::internalDrawGui(const GreenOrange &greenOrange) {
    Entity *selectedEntity = guiRoot.getSelectedEntity();
    bool open;
    
    ImGui::Begin("Inspector", &open);
    {
        if(selectedEntity != nullptr) {
            ImGui::Text(selectedEntity->getName().c_str());

            ImGui::Text(EntityTypeStrings[static_cast<int>(selectedEntity->getType())]);
            ImGui::SameLine();
            ImGui::Text("Type: ");
            ImGui::SameLine();

            if(selectedEntity->isCsgOperator()) {
                doCsgOperator(static_cast<CsgOperator&>(*selectedEntity));
            }
            else if(selectedEntity->isObject()) {
                doObject(*greenOrange.getOpenProject(), static_cast<Object&>(*selectedEntity));
            }
            else if(selectedEntity->isTransform()) {
                doTransform(static_cast<Transform&>(*selectedEntity));
            }
            else if(selectedEntity->isLight()) {
                doLight(static_cast<Light&>(*selectedEntity));
            }
            else if(selectedEntity->isMaterial()) {
                doMaterial(static_cast<Material&>(*selectedEntity));
            }
            else if(selectedEntity->isProbe()) {
                doProbe(static_cast<Probe&>(*selectedEntity));
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

void InspectorPanel::doObject(Project &project, Object &object) {
    ImGui::Text(ObjectTypeStrings[static_cast<int>(object.getType())]);
    ImGui::NewLine();

    bool isStatic = object.isStatic();
    ImGui::Checkbox("Static", &isStatic);
    object.setStatic(isStatic);

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
        auto dims = box.getDimensions();
        if(ImGui::DragFloat3("Size", glm::value_ptr(dims), 0.01f, 0.0f, DRAG_MAX))
            box.setDimensions(dims);
        break;
    }
    default:
        GO_ASSERT_ALWAYS();
        break;
    }

    ImGui::NewLine();

    Material *attachedMat = static_cast<Material*>(project.findSceneEntity(object.getMaterialId()));
    const char* matName = attachedMat ? attachedMat->getName().c_str() : "No Material";
    
    if(ImGui::BeginCombo("Material", matName, 0)) {
        int current = attachedMat ? attachedMat->getId() : -1;
        for(int i = 0; i < project.getMaterialCount(); ++i) {
            Material &mat = project.getMaterialByIndex(i);
            bool isSelected = current == mat.getId();
            if(ImGui::Selectable(mat.getName().c_str(), isSelected))
                object.attachToMaterial(mat);
            
            if(isSelected)
                // Set the initial focus when opening the combo
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void InspectorPanel::doTransform(Transform &transform) {
    ImGui::Text(TransformTypeStrings[static_cast<int>(transform.getType())]);
    ImGui::NewLine();

    switch(transform.getType()) {
    case TransformType::Translation:
    {
        Translation &trans = static_cast<Translation&>(transform);
        auto ammount = trans.getAmmount();
        if(ImGui::DragFloat3("Ammount", glm::value_ptr(ammount), 0.05f, DRAG_MIN, DRAG_MAX))
            trans.setAmmount(ammount);
        break;
    }
    case TransformType::Rotation:
    {
        Rotation &rot = static_cast<Rotation&>(transform);
        auto ammount = rot.getAmmount();
        if(ImGui::DragFloat3("Ammount", glm::value_ptr(ammount), 1.0f, -180.0f, 180.0f))
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

    bool isStatic = light.isStatic();
    ImGui::Checkbox("Static", &isStatic);
    light.setStatic(isStatic);

    switch(light.getType()) {
    case LightType::Directional:
    {
        DirectionalLight &dirLight = static_cast<DirectionalLight&>(light);
        auto dir = dirLight.getDirection();
        if(ImGui::DragFloat3("Direction", glm::value_ptr(dir), 0.01f, -1.0f, 1.0f))
            dirLight.setDirection(dir);
        break;
    }
    case LightType::Point:
    {
        PointLight &pointLight = static_cast<PointLight&>(light);
        auto pos = pointLight.getPosition();
        if(ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.05f, DRAG_MIN, DRAG_MAX))
            pointLight.setPosition(pos);
        break;
    }
    default:
        GO_ASSERT_ALWAYS();
        break;
    }

    ImGui::NewLine();

    float intensity = light.getIntensity();
    if(ImGui::DragFloat("Intensity", &intensity, 0.5f, 0.0f, DRAG_MAX))
        light.setIntensity(intensity);

    ImGui::NewLine();

    ImGuiColorEditFlags colorPickerFlags = 0;// ImGuiColorEditFlags_NoInputs;
    auto color = light.getColor();
    if(ImGui::ColorPicker3("Color", glm::value_ptr(color), colorPickerFlags))
        light.setColor(color);
}

void InspectorPanel::doMaterial(Material &material) {
    ImGui::Text("N/A");
    ImGui::NewLine();

    ImGuiColorEditFlags colorPickerFlags = 0;// ImGuiColorEditFlags_NoInputs;
    auto baseColor = material.getBaseColor();
    if(ImGui::ColorPicker3("BaseColor", glm::value_ptr(baseColor), colorPickerFlags))
        material.setBaseColor(baseColor);

    ImGui::NewLine();

    float metallic = material.getMetallic();
    if(ImGui::DragFloat("Metallic", &metallic, 0.01f, 0.0f, 1.0f))
        material.setMetallic(metallic);

    ImGui::NewLine();

    float roughness = material.getRoughness();
    if(ImGui::DragFloat("Roughness", &roughness, 0.005f, 0.0f, 1.0f))
        material.setRoughness(roughness);

    ImGui::NewLine();

    auto emissiveColor = material.getEmissiveColor();
    if(ImGui::ColorPicker3("Emissive Color", glm::value_ptr(emissiveColor), colorPickerFlags))
        material.setEmissiveColor(emissiveColor);

    ImGui::NewLine();

    float emmisiveIntensity = material.getEmissiveIntensity();
    if(ImGui::DragFloat("Emisive Intensity", &emmisiveIntensity, 0.5f, 0.0f, DRAG_MAX))
        material.setEmissiveIntensity(emmisiveIntensity);
}

void InspectorPanel::doProbe(Probe &probe) {
    ImGui::Text("N/A");
    ImGui::NewLine();

    auto pos = probe.getPosition();
    if(ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.05f, DRAG_MIN, DRAG_MAX))
        probe.setPosition(pos);
}
