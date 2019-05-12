#pragma once

//Shared data between GuiRoot and Panels. Works well with ImGui model.
struct GuiSharedData {
    int selectedSceneIdx = 0;
    bool hasChanges;
};
