#include "Project.h"

#include "../Assert.h"
#include "../GlslGenerator.h"

#include <algorithm>


uint32 Project::nextId = 0;

Project::Project(const char *path) :
    name(path), //TODO: get name from file
    path(path) {

    createScene("Scene1");
    createMaterial("DefaultMaterial");
}

void Project::createScene(const char *name) {
    scenes.emplace_back(std::make_unique<Scene>(name));
    GEN_SET_DIRTY();
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
        GEN_SET_DIRTY();
    }
}

void Project::createMaterial(const char *name) {
    materials.emplace_back(std::make_unique<Material>(generateId(), name));
    GEN_SET_DIRTY();
}

Material& Project::getMaterialByIndex(uint32 idx) {
    GO_ASSERT(idx < materials.size());
    return *materials[idx];
}

void Project::deleteMaterialByIndex(uint32 idx) {
    GO_ASSERT(idx < materials.size());
    materials.erase(materials.begin() + idx);
    GEN_SET_DIRTY();
}

Entity* Project::findSceneEntity(uint32 id) {
    for(int i = 0; i < materials.size(); ++i) {
        if(materials[i]->getId() == id) {
            return materials[i].get();
        }
    }
    return nullptr;
}

void Project::doPendingOperations() {
    for(auto &scenePtr : scenes) {
        scenePtr->doPendingOperations();
    }
}