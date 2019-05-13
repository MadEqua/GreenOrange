#pragma once

#include <memory>

#include "Project.h"
#include "../GlslGenerator.h"


class GreenOrange
{
public:
    void openProject(const char *path);

    Project* getCurrentProject() const { return currentProject.get(); }
    bool hasCurrentProject() const { return currentProject.get() != nullptr; }

    const std::string& generateCurrentProjectGlsl() const;
    const std::string& getCurrentProjectGlsl() const;

private:
    std::unique_ptr<Project> currentProject;
    GlslGenerator glslGenerator;
};