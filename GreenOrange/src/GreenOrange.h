#pragma once

#include <memory>

#include "Project.h"

class GreenOrange
{
public:
    void openProject();

    Project* getCurrentProject() const { return currentProject.get(); }
    bool hasCurrentProject() const { return currentProject.get() != nullptr; }

private:
    std::unique_ptr<Project> currentProject;
};

