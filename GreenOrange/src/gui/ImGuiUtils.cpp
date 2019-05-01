#include "ImGuiUtils.h"


namespace ImGuiUtils
{
    bool ButtonCondition(const char* label, const ImVec2& size, bool enableCondition) {
        if(!enableCondition) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }

        bool pressed = ImGui::Button(label, size);
        if(!enableCondition) {
            ImGui::PopStyleVar();
        }
        return pressed && enableCondition;
    }

    bool InputTextPopup(const char* label, const char* text, char* buffer, int bufferSize) {
        bool result = false;
        if(ImGui::BeginPopupContextItem(label)) {
            ImGui::Text(text);
            ImGui::Separator();

            if(ImGui::InputText("##edit", buffer, bufferSize, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
                ImGui::CloseCurrentPopup();
                result = true;
            }
            ImGui::Separator();

            if(ImGuiUtils::ButtonCondition("Confirm", ImVec2(120, 0), strlen(buffer))) {
                ImGui::CloseCurrentPopup();
                result = true;
            }

            ImGui::SameLine();
            if(ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        return result;
    }

    bool YesNoPopup(const char* label, const char* text) {
        bool result = false;
        if(ImGui::BeginPopupContextItem(label)) {
            ImGui::Text(text);
            ImGui::Separator();

            if(ImGui::Button("Yep", ImVec2(120, 0))) {
                result = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if(ImGui::Button("Hell No!", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        return result;
    }

    bool OkPopup(const char* label, const char* text) {
        bool result = false;
        if(ImGui::BeginPopupContextItem(label)) {
            ImGui::Text(text);
            ImGui::Separator();

            if(ImGui::Button("OK", ImVec2(120, 0))) {
                result = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        return result;
    }
}


