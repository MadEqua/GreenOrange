#pragma once

#include <vector>
#include <memory>
#include <string>

#include "../Types.h"
#include "../Assert.h"
#include "Scene.h"

class Project
{
public:
    explicit Project(const char* name);

    void addScene(const char* name);
    
    size_t getSceneCount() const { return scenes.size(); }
    Scene& getScene(uint32 idx) { GO_ASSERT(idx < scenes.size()); return *scenes[idx]; }

    const std::string& getName() const { return name; }

    void deleteScene(uint32 idx) { GO_ASSERT(idx < scenes.size()); scenes.erase(scenes.begin() + idx); }

private:
    std::string name;
    std::vector<std::unique_ptr<Scene>> scenes;

    uint32 getUnusedSceneId() const;
};

