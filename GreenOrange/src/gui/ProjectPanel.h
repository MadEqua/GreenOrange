#pragma once

#include "Panel.h"
#include "../Constants.h"

class Project;

class ProjectPanel : public Panel
{
public:
    ProjectPanel() : Panel(PanelType::Project, true) {}

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;

    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
    mutable char stringBuffer[STRING_BUFFER_MAX_SIZE] = "";
};