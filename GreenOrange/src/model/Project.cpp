#include "Project.h"


Project::Project(const char *path) :
    name(path), //TODO: get name from file
    path(path) {
    addScene("Scene1");
    addScene("Scene2");
    addScene("Scene3");
    addScene("Scene4");
    addScene("Scene5");
}

void Project::addScene(const char *name) {
    scenes.emplace_back(name);
}

Scene& Project::getSceneByIndex(uint32 idx) { 
    GO_ASSERT(idx < scenes.size());
    return scenes[idx];
}

void Project::deleteSceneByIndex(uint32 idx) { 
    GO_ASSERT(idx < scenes.size()); 
    scenes.erase(scenes.begin() + idx);
}