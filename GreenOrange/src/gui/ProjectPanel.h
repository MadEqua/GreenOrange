#pragma once

#include "../Constants.h"

class Project;

class ProjectPanel
{
public:
    void drawGui(Project &project);

private:
    int selectedSceneIdx = 0; //TODO: this should be shared with other Panels
    char buffer[STRING_MAX_SIZE] = "";
};
