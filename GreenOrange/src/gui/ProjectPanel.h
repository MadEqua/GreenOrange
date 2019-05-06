#pragma once

#include "../Constants.h"

class Project;

class ProjectPanel
{
public:
    void drawGui(Project &project, int &selectedSceneIdx);

private:
    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
    mutable char stringBuffer[STRING_BUFFER_MAX_SIZE] = "";
};