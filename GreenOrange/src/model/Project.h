#pragma once

#include <vector>
#include <string>

#include "../Types.h"
#include "../Assert.h"
#include "Scene.h"


class Project
{
public:
    explicit Project(const char *path);

    void addScene(const char *name);
    
    size_t getSceneCount() const { return scenes.size(); }
    Scene& getSceneByIndex(uint32 idx);
    void deleteSceneByIndex(uint32 idx);

    const std::string& getName() const { return name; }

private:
    std::string path;
    std::string name;
    std::vector<Scene> scenes;
};

