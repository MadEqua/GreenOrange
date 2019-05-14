#include "GreenOrange.h"

#include "../Assert.h"


void GreenOrange::openProject(const char *path) {
    //TODO
    _openProject = std::make_unique<Project>("Test project");
}

void GreenOrange::closeProject() {
    //TODO: check if we have unsaved changes
    _openProject.release();
}

const std::string& GreenOrange::generateCurrentProjectGlsl() const {
    GO_ASSERT(_openProject)
    return glslGenerator.generate(*_openProject);
}

const std::string& GreenOrange::getCurrentProjectGlsl() const {
    GO_ASSERT(_openProject)
    return glslGenerator.getLastGenerated();
}