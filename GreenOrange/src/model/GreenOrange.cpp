#include "GreenOrange.h"
#include "../GlslGenerator.h"


void GreenOrange::openProject(const char *path) {
    //TODO
    _openProject = std::make_unique<Project>("Test project");
    GEN_SET_DIRTY();;
}

void GreenOrange::closeProject() {
    //TODO: check if we have unsaved changes
    _openProject.release();
}