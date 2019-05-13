#pragma once

#include "../Constants.h"

class Project;

class ProjectPanel
{
public:
    void drawGui(Project &project) const;

private:
    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
    mutable char stringBuffer[STRING_BUFFER_MAX_SIZE] = "";
};