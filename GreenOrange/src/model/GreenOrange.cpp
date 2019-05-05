#include "GreenOrange.h"

#include "DataRepo.h"


GreenOrange::GreenOrange() {
    initDataRepo();
}

void GreenOrange::openProject() {
    //TODO
    currentProject = std::make_unique<Project>("Test project");
}