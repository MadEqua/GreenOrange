#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>

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
    void setName(const char* newName) { name = newName; GEN_SET_DIRTY(); }

    //-----------------------------------------
    //CSG tree
    TreeNode<SceneEntity>& getCsgTreeRootNode() { return *csgTreeRoot; }
    CsgOperator& getCsgRootOperator() { return static_cast<CsgOperator&>(csgTreeRoot->getPayload()); }

    void createCsgOperator(const char *name, CsgType type, TreeNode<SceneEntity> &parent);
    void createObject(const char *name, ObjectType type, TreeNode<SceneEntity> &parent);

    void deleteCsgTreeNode(TreeNode<SceneEntity> &toDelete);
    void moveCsgTreeNode(TreeNode<SceneEntity> &toMove, TreeNode<SceneEntity> &destination);
    void deleteCsgTreeNodeChildren(TreeNode<SceneEntity> &toDeleteChildren);

    //-----------------------------------------
    //Transform trees
    size_t getTransformTreeCount() const { return transformTreeRoots.size(); }
    TreeNode<Transform>& getTransformTreeRootNodeByIndex(uint32 i) { GO_ASSERT(i < transformTreeRoots.size()); return *transformTreeRoots[i]; }

    void createRootTransform(const char *name, TransformType type);
    void deleteRootTransform(uint32 treeIndex);
    void createTransform(const char *name, TransformType type, TreeNode<Transform> &parent);

    void deleteTransformTreeNode(uint32 treeIndex, TreeNode<Transform> &toDelete);
    void moveTransformTreeNode(uint32 toMoveTreeIndex, TreeNode<Transform> &toMove, uint32 destinationTreeIndex, TreeNode<Transform> &destination);
    void attachObjectToTransformTreeNode(Object &object, TreeNode<Transform> &transform);
    void detachObjectToTransformTreeNode(Object &object);
    void deleteTransformTreeNodeChildren(uint32 treeIndex, TreeNode<Transform> &toDeleteChildren);

    //-----------------------------------------
    //Current selection
    void setSelectedEntity(SceneEntity &ent) { selectedEntityId = ent.getId(); }
    SceneEntity* getSelectedEntity();
    void clearSelectedEntity() { selectedEntityId = -1; }

    //-----------------------------------------
    //SceneEntity
    SceneEntity* findSceneEntity(uint32 id);

    //-----------------------------------------
    //Deletion postponing
    void doPendingOperations();

private:
    std::string name;

    //All SceneEntities are owned here. The trees only have references.
    std::vector<std::unique_ptr<SceneEntity>> sceneEntities;

    std::unique_ptr<TreeNode<SceneEntity>> csgTreeRoot;
    std::vector<std::unique_ptr<TreeNode<Transform>>> transformTreeRoots;

    uint32 nextId = 0;
    uint32 selectedEntityId = -1;

    //-----------------------------------------
    //Aux functions
    uint32 generateId() { return nextId++; }
    std::unique_ptr<Transform> internalCreateTransform(const char *name, TransformType type);

    //-----------------------------------------
    //Internal SceneEntity management
    typename std::vector<std::unique_ptr<SceneEntity>>::iterator getIteratorToSceneEntityId(uint32 id);
    bool deleteSceneEntity(SceneEntity &toDelete);

    //-----------------------------------------
    //Deletion postponing
    std::vector<TreeNode<SceneEntity>*> pendingDeleteCsgTreeNode;
    std::vector<std::pair<uint32, TreeNode<Transform>*>> pendingDeleteTransformTreeNode;
};

