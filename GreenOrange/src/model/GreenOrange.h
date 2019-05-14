#pragma once

#include <memory>

#include "Project.h"
#include "../GlslGenerator.h"


class GreenOrange
{
public:
    void openProject(const char *path);
    void closeProject();

    Project* getOpenProject() const { return _openProject.get(); }
    bool hasOpenProject() const { return _openProject.get() != nullptr; }

    const std::string& generateCurrentProjectGlsl() const;
    const std::string& getCurrentProjectGlsl() const;

private:
    std::unique_ptr<Project> _openProject;
    GlslGenerator glslGenerator;
};