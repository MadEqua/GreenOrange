#include "ProjectPanel.h"

#include <string.h>
#include <imgui.h>

#include "../model/Project.h"
#include "../model/Scene.h"
#include "GuiSharedData.h"
#include "imgui/ImGuiUtils.h"

static bool sceneItemsGetter(void* data, int i, const char** string) {
    if(data != nullptr) {
        Project *project = static_cast<Project*>(data);
        if(i < project->getSceneCount()) {
            (*string) = project->getScene(i).getName().c_str();
            return true;
        }
    }
    return false;
}

void ProjectPanel::drawGui(Project &project, int &selectedSceneIdx) {
    //ImGui::SetNextWindowPos(ImVec2(0, 20)); //TODO: de-hardcode
    ImGui::Begin("Project", 0, 0);
    {
        ImGui::ListBox("##sceneList", &selectedSceneIdx, sceneItemsGetter, &project, static_cast<int>(project.getSceneCount()));
        
        if(ImGui::Button("Create Scene")) {
            ImGui::OpenPopup("Create Scene");
        }
        if(ImGuiUtils::InputTextPopup("Create Scene", "Enter a name for the new scene.", buffer, STRING_MAX_SIZE)) {
            project.addScene(buffer);
            buffer[0] = 0;
        }

        ImGui::SameLine();
        if(ImGui::Button("Rename Scene")) {
            strcpy_s(buffer, project.getScene(selectedSceneIdx).getName().c_str());
            ImGui::OpenPopup("Rename Scene");
        }
        if(ImGuiUtils::InputTextPopup("Rename Scene", "Enter a new name for the scene.", buffer, STRING_MAX_SIZE)) {
            project.getScene(selectedSceneIdx).setName(buffer);
            buffer[0] = 0;
        }

        ImGui::SameLine();
        if(ImGui::Button("Delete Scene")) {
            ImGui::OpenPopup("Delete Scene");
        }
        if(ImGuiUtils::YesNoPopup("Delete Scene", "Delete the scene?\nThis operation cannot be undone!")) {
            project.deleteScene(selectedSceneIdx);
        }
    }
    ImGui::End();
}