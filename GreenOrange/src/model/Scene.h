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

class Entity;


class Scene
{
public:
    explicit Scene(const char* name);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; GEN_SET_DIRTY(); }

    //-----------------------------------------
    //CSG tree
    TreeNode<Entity>& getCsgTreeRootNode() { return *csgTreeRoot; }
    CsgOperator& getCsgRootOperator() { return static_cast<CsgOperator&>(csgTreeRoot->getPayload()); }

    void createCsgOperator(const char *name, CsgType type, TreeNode<Entity> &parent);
    void createObject(const char *name, ObjectType type, TreeNode<Entity> &parent);

    void deleteCsgTreeNode(TreeNode<Entity> &toDelete);
    void moveCsgTreeNode(TreeNode<Entity> &toMove, TreeNode<Entity> &destination);
    void swapCsgTreeSiblingNodes(TreeNode<Entity> &node1, TreeNode<Entity> &node2);
    void deleteCsgTreeNodeChildren(TreeNode<Entity> &toDeleteChildren);

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
    //Entity
    Entity* findSceneEntity(uint32 id);

    //-----------------------------------------
    //Deletion postponing
    void doPendingOperations();

private:
    std::string name;

    //All SceneEntities are owned here. The trees and other lists only have references.
    std::vector<std::unique_ptr<Entity>> sceneEntities;

    std::unique_ptr<TreeNode<Entity>> csgTreeRoot;

    //Every transform tree will be a direct child of this one, allowing for easy moving between any nodes. This root will be hidden on the UI.
    std::unique_ptr<TreeNode<Transform>> transformTreeDummyRoot;

    std::vector<Light*> lights;

    //-----------------------------------------
    //Aux functions
    std::unique_ptr<Object> internalCreateObject(const char *name, ObjectType type);
    std::unique_ptr<Transform> internalCreateTransform(const char *name, TransformType type);
    std::unique_ptr<Light> internalCreateLight(const char *name, LightType type);

    //-----------------------------------------
    //Internal Entity management
    typename std::vector<std::unique_ptr<Entity>>::iterator getIteratorToSceneEntityId(uint32 id);
    bool deleteSceneEntity(Entity &toDelete);

    //-----------------------------------------
    //Deletion postponing
    std::vector<TreeNode<Entity>*> pendingDeleteCsgTreeNodes;
    std::vector<std::pair<uint32, TreeNode<Transform>*>> pendingDeleteTransformTreeNodes;
    std::vector<Light*> pendingDeleteLights;
};
