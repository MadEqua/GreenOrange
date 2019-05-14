#pragma once

#include <memory>

#include "Project.h"


class GreenOrange
{
public:
    void openProject(const char *path);
    void closeProject();

    Project* getOpenProject() const { return _openProject.get(); }
    bool hasOpenProject() const { return _openProject.get() != nullptr; }

private:
    std::unique_ptr<Project> _openProject;
};