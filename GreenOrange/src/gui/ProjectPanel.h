#pragma once

#include "../Constants.h"

class Project;

class ProjectPanel
{
public:
    void drawGui(Project &project, int &selectedSceneIdx);

private:
    mutable char buffer[STRING_MAX_SIZE] = "";
};