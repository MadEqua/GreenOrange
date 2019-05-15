#pragma once

#include <string>
#include <functional>

#include "../DataRepo.h"
#include "CsgOperator.h"
#include "../GlslGenerator.h"

class Object;
class SceneEntity;


class Scene
{
public:
    explicit Scene(const char* name);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; GEN_SET_DIRTY() }

    CsgOperator& getRootCsgOperator() { return unionOperator; }

    void createCsgOperator(const char *name, CsgType type, CsgOperator &parent);
    void deleteCsgOperator(CsgOperator &toDelete);
    void moveCsgOperator(CsgOperator &toMove, CsgOperator &destination);
    void clearCsgOperatorChildObjects(CsgOperator &toClear);
    void clearCsgOperatorChildCsgOperators(CsgOperator &toClear);
    void deleteCsgOperatorChildren(CsgOperator &toClear);

    void createObject(const char *name, ObjectType type, CsgOperator &parent);
    void deleteObject(Object &toDelete);
    void moveObject(Object &toMove, CsgOperator &destination);

    uint32 generateId() { return nextId++; }

    void setSelectedEntity(SceneEntity &ent) { selectedEntityId = ent.getId(); }
    SceneEntity* getSelectedEntity();
    void clearSelectedEntity() { selectedEntityId = -1; }

    //The visitFunction will receive the Node and its parent, in this order
    //If visitFunction returns true the traversal will stop
    static void traverseTreeBfs(CsgOperator &root, const std::function<bool(SceneEntity&, CsgOperator*)> &visitFunction);
    static void traverseTreeDfs(CsgOperator &root, const std::function<bool(SceneEntity&)> &visitFunction);

private:
    std::string name;

    //A Scene has a Union CsgOperator by default
    CsgOperator unionOperator;

    uint32 nextId;

    uint32 selectedEntityId = -1;

    bool isCsgOperatorDescendentOf(CsgOperator &op1, CsgOperator &op2);
};

