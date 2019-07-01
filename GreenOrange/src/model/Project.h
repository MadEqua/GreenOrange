#pragma once

#include <vector>
#include <memory>
#include <string>

#include "../Types.h"
#include "Scene.h"
#include "Material.h"


class Project
{
public:
    explicit Project(const char *path);

    const std::string& getName() const { return name; }
    
    void createScene(const char *name);
    size_t getSceneCount() const { return scenes.size(); }
    Scene& getSceneByIndex(uint32 idx);
    void deleteSceneByIndex(uint32 idx);

    void setSelectedSceneIdx(uint32 idx) { selectedSceneIdx = idx; }
    uint32 getSelectedSceneIdx() const { return selectedSceneIdx; }
    Scene& getSelectedScene() { return getSceneByIndex(selectedSceneIdx); }

    void createMaterial(const char *name);
    size_t getMaterialCount() const { return materials.size(); }
    Material& getMaterialByIndex(uint32 idx);
    void deleteMaterialByIndex(uint32 idx);

    Entity* findSceneEntity(uint32 id);

    void doPendingOperations();

    void setOnlyPreviewStaticObjects(bool b) { onlyPreviewStaticObjects = b; GEN_SET_DIRTY(); }
    bool getOnlyPreviewStaticObjects() const { return onlyPreviewStaticObjects; }

    static uint32 generateId() { return nextId++; }

private:
    std::string path;
    std::string name;
    std::vector<std::unique_ptr<Scene>> scenes;
    std::vector<std::unique_ptr<Material>> materials;

    uint32 selectedSceneIdx = 0;
    static uint32 nextId;

    bool onlyPreviewStaticObjects = false;
};

