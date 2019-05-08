#include "InspectorPanel.h"

#include <limits>

#include <imgui.h>

void InspectorPanel::drawGui() {
    static float vec[4] = {0.10f, 0.20f, 0.30f, 0.44f};
    ImGui::Begin("Object Inspector", 0, 0);
    {
        ImGui::DragFloat3("World Position", vec, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
        ImGui::DragFloat3("Orientation", vec, 0.01f, 0.0f, 359.0f);
        ImGui::DragFloat3("Size (boxes)", vec, 0.01f, 0.0f, std::numeric_limits<float>::max());
        ImGui::DragFloat("Radius (spheres)", vec, 0.01f, 0.0f, std::numeric_limits<float>::max());
    }
    ImGui::End();
}