#pragma once

#include <string>

#include "CsgOperator.h"
#include "Transform.h"
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

    //-----------------------------------------
    //CSG tree
    TreeNode<SceneEntity>& getCsgTreeRootNode() { return csgTreeRoot; }
    CsgOperator& getCsgRootOperator() { return static_cast<CsgOperator&>(csgTreeRoot.getPayload()); }

    void createCsgOperator(const char *name, CsgType type, TreeNode<SceneEntity> &parent);
    void createObject(const char *name, ObjectType type, TreeNode<SceneEntity> &parent);

    void deleteCsgTreeNode(TreeNode<SceneEntity> &toDelete);
    void moveCsgTreeNode(TreeNode<SceneEntity> &toMove, TreeNode<SceneEntity> &destination);
    void deleteCsgTreeNodeChildren(TreeNode<SceneEntity> &toDelete);

    //-----------------------------------------
    //Transform tree
    TreeNode<Transform>& getTransformTreeRootNode() { return transformTreeRoot; }
    Transform& getTransformRootOperator() { return static_cast<Transform&>(transformTreeRoot.getPayload()); }

    void createTransform(const char *name, TransformType type, TreeNode<Transform> &parent);

    void deleteTransformTreeNode(TreeNode<Transform> &toDelete);
    void moveTransformTreeNode(TreeNode<Transform> &toMove, TreeNode<Transform> &destination);
    void deleteTransformTreeNodeChildren(TreeNode<Transform> &toDelete);

    uint32 generateId() { return nextId++; }

    void setSelectedEntity(SceneEntity &ent) { selectedEntityId = ent.getId(); }
    SceneEntity* getSelectedEntity();
    void clearSelectedEntity() { selectedEntityId = -1; }

private:
    std::string name;

    TreeNode<SceneEntity> csgTreeRoot;
    TreeNode<Transform> transformTreeRoot;

    uint32 nextId;
    uint32 selectedEntityId = -1;
};

