#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "CsgOperator.h"
#include "Transform.h"
#include "Light.h"
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
    void swapCsgTreeSiblingNodes(TreeNode<SceneEntity> &node1, TreeNode<SceneEntity> &node2);
    void deleteCsgTreeNodeChildren(TreeNode<SceneEntity> &toDeleteChildren);

    //-----------------------------------------
    //Transform trees
    size_t getTransformTreeCount() const { return transformTreeDummyRoot->getChildCount(); }
    TreeNode<Transform>& getTransformTreeRootNodeByIndex(uint32 i) { GO_ASSERT(i < transformTreeDummyRoot->getChildCount()); return transformTreeDummyRoot->getChildByIndex(i); }

    void createRootTransform(const char *name, TransformType type);
    void createTransform(const char *name, TransformType type, TreeNode<Transform> &parent);

    void deleteTransformTreeNode(uint32 treeIndex, TreeNode<Transform> &toDelete);
    void moveTransformTreeNode(uint32 toMoveTreeIndex, TreeNode<Transform> &toMove, TreeNode<Transform> &destination);
    void attachObjectToTransformTreeNode(Object &object, TreeNode<Transform> &transform);
    void detachObjectToTransformTreeNode(Object &object);
    void deleteTransformTreeNodeChildren(uint32 treeIndex, TreeNode<Transform> &toDeleteChildren);

    //-----------------------------------------
    //Lights
    void createLight(const char *name, LightType type);
    void deleteLight(Light &light);
    size_t getLightCount() const { return lights.size(); }
    Light& getLightByIndex(uint32 i) { GO_ASSERT(i < lights.size()); return *lights[i]; }
    uint32 getDirLightCount() const;
    uint32 getPointLightCount() const;

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

    //All SceneEntities are owned here. The trees and other lists only have references.
    std::vector<std::unique_ptr<SceneEntity>> sceneEntities;

    std::unique_ptr<TreeNode<SceneEntity>> csgTreeRoot;

    //Every transform tree will be a direct child of this one, allowing for easy moving between any nodes. This root will be hidden on the UI.
    std::unique_ptr<TreeNode<Transform>> transformTreeDummyRoot;

    std::vector<Light*> lights;

    uint32 nextId = 0;
    uint32 selectedEntityId = -1;

    //-----------------------------------------
    //Aux functions
    uint32 generateId() { return nextId++; }
    std::unique_ptr<Object> internalCreateObject(const char *name, ObjectType type);
    std::unique_ptr<Transform> internalCreateTransform(const char *name, TransformType type);
    std::unique_ptr<Light> internalCreateLight(const char *name, LightType type);

    //-----------------------------------------
    //Internal SceneEntity management
    typename std::vector<std::unique_ptr<SceneEntity>>::iterator getIteratorToSceneEntityId(uint32 id);
    bool deleteSceneEntity(SceneEntity &toDelete);

    //-----------------------------------------
    //Deletion postponing
    std::vector<TreeNode<SceneEntity>*> pendingDeleteCsgTreeNodes;
    std::vector<std::pair<uint32, TreeNode<Transform>*>> pendingDeleteTransformTreeNodes;
    std::vector<Light*> pendingDeleteLights;
};
