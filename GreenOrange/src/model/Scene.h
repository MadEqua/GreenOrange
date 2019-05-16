#pragma once

#include <string>

#include "CsgOperator.h"
#include "../Tree.h"
#include "../DataRepo.h"
#include "../GlslGenerator.h"

class SceneEntity;


class Scene
{
public:
    explicit Scene(const char* name);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; GEN_SET_DIRTY() }

    TreeNode<SceneEntity>& getRootTreeNode() { return treeRoot; }
    CsgOperator& getRootCsgOperator() { return static_cast<CsgOperator&>(treeRoot.getPayload()); }

    void createCsgOperator(const char *name, CsgType type, TreeNode<SceneEntity> &parent);
    void createObject(const char *name, ObjectType type, TreeNode<SceneEntity> &parent);

    void deleteNode(TreeNode<SceneEntity> &toDelete);
    void moveNode(TreeNode<SceneEntity> &toMove, TreeNode<SceneEntity> &destination);
    void deleteNodeChildren(TreeNode<SceneEntity> &toDelete);

    uint32 generateId() { return nextId++; }

    void setSelectedEntity(SceneEntity &ent) { selectedEntityId = ent.getId(); }
    SceneEntity* getSelectedEntity();
    void clearSelectedEntity() { selectedEntityId = -1; }

private:
    std::string name;

    TreeNode<SceneEntity> treeRoot;

    uint32 nextId;
    uint32 selectedEntityId = -1;
};

