#include "ProjectPanel.h"

#include <string.h>
#include <imgui.h>

#include "../model/Project.h"
#include "../model/Scene.h"
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

void ProjectPanel::drawGui(Project &project) const {
    ImGui::Begin("Project", 0, 0);
    {
        bool openCreatePopup = false;
        bool openRenamePopup = false;
        bool openDeletePopup = false;

        int selectedSceneIdx = project.getSelectedSceneIdx();
        ImGui::ListBox("##sceneList", &selectedSceneIdx, sceneItemsGetter, &project, static_cast<int>(project.getSceneCount()));
        project.setSelectedSceneIdx(selectedSceneIdx);

        //This will be used to context menus on the selected scene, instead of the clicked scene...
        //Need to figure out how to detect clicks on ListBoxes.
        Scene &selectedScene = project.getSelectedScene();

        if(ImGui::BeginPopupContextItem("##sceneList")) {
            if(ImGui::Selectable("New Scene")) {
                openCreatePopup = true;
            }
            if(ImGui::Selectable("Rename")) {
                openRenamePopup = true;
            }
            if(ImGui::Selectable("Delete")) {
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

        if(ImGuiUtils::InputTextPopup("New Scene", "Enter a name for the new scene.", inputBuffer, INPUT_STRING_MAX_SIZE)) {
            project.addScene(inputBuffer);
        }

        const char *newNameString = "Enter a new name for %s.";
        sprintf_s(stringBuffer, newNameString, selectedScene.getName().c_str());
        if(ImGuiUtils::InputTextPopup("Rename Scene", stringBuffer, inputBuffer, INPUT_STRING_MAX_SIZE)) {
            project.getSceneByIndex(selectedSceneIdx).setName(inputBuffer);
        }

        const char *deleteSceneString = "Delete the scene %s?\nThis operation cannot be undone!";
        sprintf_s(stringBuffer, deleteSceneString, selectedScene.getName().c_str());
        if(ImGuiUtils::YesNoPopup("Delete Scene", stringBuffer)) {
            project.deleteSceneByIndex(selectedSceneIdx);
        }
    }
    ImGui::End();
}