#include "GreenOrange.h"

#include "DataRepo.h"


GreenOrange::GreenOrange() {
    initDataRepo();
}

void GreenOrange::openProject(const char *path) {
    //TODO
    currentProject = std::make_unique<Project>("Test project");
}