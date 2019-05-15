#include "Project.h"

#include "../Assert.h"
#include "../GlslGenerator.h"


Project::Project(const char *path) :
    name(path), //TODO: get name from file
    path(path) {
    addScene("Scene1");
}

void Project::addScene(const char *name) {
    scenes.emplace_back(std::make_unique<Scene>(name));
    GEN_SET_DIRTY()
}

Scene& Project::getSceneByIndex(uint32 idx) { 
    GO_ASSERT(idx < scenes.size());
    return *scenes[idx];
}

void Project::deleteSceneByIndex(uint32 idx) { 
    GO_ASSERT(idx < scenes.size());
    if(scenes.size() > 1) {
        scenes.erase(scenes.begin() + idx);
        if(selectedSceneIdx == idx)
            selectedSceneIdx = 0;
        GEN_SET_DIRTY()
    }
}