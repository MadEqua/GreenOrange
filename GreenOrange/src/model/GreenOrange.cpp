#include "GreenOrange.h"

#include "../Assert.h"


void GreenOrange::openProject(const char *path) {
    //TODO
    currentProject = std::make_unique<Project>("Test project");
}

const std::string& GreenOrange::generateCurrentProjectGlsl() const {
    GO_ASSERT(currentProject)
    return glslGenerator.generate(*currentProject);
}

const std::string& GreenOrange::getCurrentProjectGlsl() const {
    GO_ASSERT(currentProject)
    return glslGenerator.getLastGenerated();
}