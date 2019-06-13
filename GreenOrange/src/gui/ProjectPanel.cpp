#include "ProjectPanel.h"

#include <string.h>
#include <imgui.h>

#include "../model/GreenOrange.h"
#include "../model/Project.h"
#include "../model/Scene.h"
#include "GuiRoot.h"
#include "imgui/ImGuiUtils.h"


static bool sceneItemsGetter(void* data, int i, const char** string) {
    if(data != nullptr) {
        Project *project = static_cast<Project*>(data);
        if(i < project->getSceneCount()) {
            (*string) = project->getSceneByIndex(i).getName().c_str();
            return true;
        }
    }
    return false;
}

ProjectPanel::ProjectPanel(GuiRoot &guiRoot) : 
    Panel(guiRoot, PanelType::Project, true) {
}

bool ProjectPanel::internalDrawGui(const GreenOrange &greenOrange) {
    Project &project = *greenOrange.getOpenProject();
    bool open;

    ImGui::Begin("Project", &open);
    {
        doSceneList(project);
        ImGui::NewLine();
        doMaterialList(project);
    }
    ImGui::End();

    return open;
}

void ProjectPanel::doSceneList(Project &project) const {
    bool openCreatePopup = false;
    bool openRenamePopup = false;
    bool openDeletePopup = false;

    int selectedSceneIdx = project.getSelectedSceneIdx();
    ImGui::ListBox("##sceneList", &selectedSceneIdx, sceneItemsGetter, &project, static_cast<int>(project.getSceneCount()));
    project.setSelectedSceneIdx(selectedSceneIdx);

    //This will be used to context menus on the selected scene, instead of the clicked scene...
    //Need to figure out how to detect clicks on ListBox items.
    Scene &selectedScene = project.getSelectedScene();

    if(ImGui::BeginPopupContextItem("##sceneList")) {
        if(ImGui::Selectable("New Scene")) {
            openCreatePopup = true;
        }
        if(ImGui::Selectable("Rename")) {
            openRenamePopup = true;
        }
        if(project.getSceneCount() > 1 && ImGui::Selectable("Delete")) {
            openDeletePopup = true;
        }
        ImGui::EndPopup();
    }

    if(ImGui::Button("New Scene") || openCreatePopup) {
        inputBuffer[0] = 0;
        ImGui::OpenPopup("New Scene");
    }
    if(openRenamePopup) {
        strcpy_s(inputBuffer, selectedScene.getName().c_str());
        ImGui::OpenPopup("Rename Scene");
    }
    if(openDeletePopup) {
        ImGui::OpenPopup("Delete Scene");
    }

    if(ImGuiUtils::InputTextPopup("New Scene", "Enter a name for the new Scene.", inputBuffer, INPUT_STRING_MAX_SIZE)) {
        project.createScene(inputBuffer);
    }

    const char *newNameString = "Enter a new name for %s.";
    sprintf_s(stringBuffer, newNameString, selectedScene.getName().c_str());
    if(ImGuiUtils::InputTextPopup("Rename Scene", stringBuffer, inputBuffer, INPUT_STRING_MAX_SIZE)) {
        project.getSceneByIndex(selectedSceneIdx).setName(inputBuffer);
    }

    const char *deleteSceneString = "Delete the Scene %s?\nThis operation cannot be undone!";
    sprintf_s(stringBuffer, deleteSceneString, selectedScene.getName().c_str());
    if(ImGuiUtils::YesNoPopup("Delete Scene", stringBuffer)) {
        project.deleteSceneByIndex(selectedSceneIdx);
    }
}


void ProjectPanel::doMaterialList(Project &project) const {
    if(ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto selectedEntity = guiRoot.getSelectedEntity();

        for(uint32 i = 0; i < project.getMaterialCount(); ++i) {
            auto &material = project.getMaterialByIndex(i);
            ImGui::PushID(material.getId());
            if(ImGui::Selectable(material.getName().c_str(), selectedEntity && material == *selectedEntity)) {
                guiRoot.setSelectedEntity(material);
            }
            doMaterialContextMenu(project, material, i);
            ImGui::PopID();
        }

        ImGui::NewLine();
        if(ImGui::Button("New Material")) {
            inputBuffer[0] = 0;
            ImGui::OpenPopup("New Material");
        }

        if(ImGuiUtils::InputTextPopup("New Material", "Enter a name for the new Material.", inputBuffer, INPUT_STRING_MAX_SIZE)) {
            project.createMaterial(inputBuffer);
        }
    }
}

void ProjectPanel::doMaterialContextMenu(Project &project, Material &material, uint32 index) const {
    bool openRenamePopup = false;
    bool openDeletePopup = false;

    if(ImGui::BeginPopupContextItem("newMaterialContext")) {
        if(ImGui::Selectable("Rename")) {
            openRenamePopup = true;
        }
        if(ImGui::Selectable("Delete")) {
            openDeletePopup = true;
        }
        ImGui::EndPopup();
    }

    if(openRenamePopup) {
        strcpy_s(inputBuffer, material.getName().c_str());
        ImGui::OpenPopup("Rename Material");
    }
    if(openDeletePopup) {
        ImGui::OpenPopup("Delete Material");
    }

    const char *newNameString = "Enter a new name for %s.";
    sprintf_s(stringBuffer, newNameString, material.getName().c_str());
    if(ImGuiUtils::InputTextPopup("Rename Material", stringBuffer, inputBuffer, INPUT_STRING_MAX_SIZE)) {
        material.setName(inputBuffer);
    }

    const char *deleteSceneString = "Delete the Material %s?\nThis operation cannot be undone!";
    sprintf_s(stringBuffer, deleteSceneString, material.getName().c_str());
    if(ImGuiUtils::YesNoPopup("Delete Material", stringBuffer)) {
        project.deleteMaterialByIndex(index);
    }
}
