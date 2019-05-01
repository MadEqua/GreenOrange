#pragma once

#include <imgui.h>

namespace ImGuiUtils
{
    bool ButtonCondition(const char* label, const ImVec2& size, bool enableCondition);
    bool InputTextPopup(const char* label, const char* text, char* buffer, int bufferSize);
    bool YesNoPopup(const char* label, const char* text);
    bool OkPopup(const char* label, const char* text);
};

