#pragma once

#include "Panel.h"
#include "../Constants.h"
#include "../Types.h"

class Project;
class Material;


class ProjectPanel : public Panel
{
public:
    ProjectPanel(GuiRoot &guiRoot);

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;

    void doSceneList(Project &project) const;
    void doMaterialList(Project &project) const;
    void doMaterialContextMenu(Project &project, Material &material, uint32 index) const;

    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
    mutable char stringBuffer[STRING_BUFFER_MAX_SIZE] = "";
};