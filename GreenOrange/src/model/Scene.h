#pragma once

#include <string>
#include <functional>

#include "../DataRepo.h"
#include "CsgOperator.h"

class Object;
class SceneEntity;


class Scene
{
public:
    explicit Scene(const char* name);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

    CsgOperator& getRootCsgOperator() { return unionOperator; }

    void createCsgOperator(const char *name, CsgType type, CsgOperator &parent);
    void deleteCsgOperator(CsgOperator &toDelete);
    void moveCsgOperator(CsgOperator &toMove, CsgOperator &destination);

    void createObject(const char *name, ObjectType type, CsgOperator &parent);
    void deleteObject(Object &toDelete);
    void moveObject(Object &toMove, CsgOperator &destination);

    uint32 generateId() { return nextId++; }

    void setSelectedEntity(SceneEntity &ent) { selectedEntity = &ent; }
    SceneEntity* getSelectedEntity() const { return selectedEntity; }
    bool hasSelectedEntity() const { return selectedEntity != nullptr; }
    void clearSelectedEntity() { selectedEntity = nullptr; }

    //The visitFunction will receive the Node and its parent, in this order
    //If visitFunction returns true the traversal will stop
    void traverseTreeBfs(CsgOperator &root, const std::function<bool(SceneEntity&, CsgOperator*)> &visitFunction);
    void traverseTreeDfs(CsgOperator &root, const std::function<bool(SceneEntity&)> &visitFunction);

private:
    std::string name;

    //A Scene has a Union CsgOperator by default
    CsgOperator unionOperator;

    uint32 nextId;

    SceneEntity* selectedEntity = nullptr;

    bool isCsgOperatorDescendentOf(CsgOperator &op1, CsgOperator &op2);
};

