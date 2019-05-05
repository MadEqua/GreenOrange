#include "Project.h"


Project::Project(const char* name) :
    name(name) {
    addScene("Scene1");
    addScene("Scene2");
    addScene("Scene3");
    addScene("Scene4");
    addScene("Scene5");
}

void Project::addScene(const char* name) {
    scenes.emplace_back(std::make_unique<Scene>(getUnusedSceneId(), name));
}

Scene& Project::getScene(uint32 idx) { 
    GO_ASSERT(idx < scenes.size());
    return *scenes[idx];
}

void Project::deleteScene(uint32 idx) { 
    GO_ASSERT(idx < scenes.size()); 
    scenes.erase(scenes.begin() + idx);
}

uint32 Project::getUnusedSceneId() const {
    int maxId = -1;
    for(auto &scenePtr : scenes) {
        if(scenePtr->getId() > maxId)
            maxId = scenePtr->getId();
    }
    return maxId + 1;
}
