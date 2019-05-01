#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Scene.h"

class Project
{
public:
    explicit Project(const char* name);

    void addScene(const char* name);
    
    int getSceneCount() { return scenes.size(); }
    Scene& getScene(int i) { return *scenes[i]; }

    const std::string& getName() const { return name; }

    void deleteScene(int i) { scenes.erase(scenes.begin() + i); }

private:
    std::string name;
    std::vector<std::unique_ptr<Scene>> scenes;

    int getUnusedSceneId() const;
};

