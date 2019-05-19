#pragma once

#include <vector>
#include <memory>
#include <string>

#include "../Types.h"
#include "Scene.h"


class Project
{
public:
    explicit Project(const char *path);

    void addScene(const char *name);

    const std::string& getName() const { return name; }
    
    size_t getSceneCount() const { return scenes.size(); }
    Scene& getSceneByIndex(uint32 idx);
    void deleteSceneByIndex(uint32 idx);

    void setSelectedSceneIdx(uint32 idx) { selectedSceneIdx = idx; }
    uint32 getSelectedSceneIdx() const { return selectedSceneIdx; }
    Scene& getSelectedScene() { return getSceneByIndex(selectedSceneIdx); }

    void doPendingOperations();

private:
    std::string path;
    std::string name;
    std::vector<std::unique_ptr<Scene>> scenes;

    uint32 selectedSceneIdx = 0;
};

