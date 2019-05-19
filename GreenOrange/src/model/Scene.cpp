#include "Scene.h"

#include "SceneEntity.h"
#include "Object.h"
#include "Transform.h"


Scene::Scene(const char* name) :
    name(name) {

    sceneEntities.emplace_back(std::make_unique<CsgOperator>(generateId(), "Root Union", CsgType::Union));
    csgTreeRoot = std::make_unique<TreeNode<SceneEntity>>(*sceneEntities[0]);
}

void Scene::createCsgOperator(const char *name, CsgType type, TreeNode<SceneEntity> &parent) {
    std::unique_ptr<CsgOperator> ptr = std::make_unique<CsgOperator>(generateId(), name, type);
    parent.createChild(*ptr);
    sceneEntities.emplace_back(std::move(ptr));
    GEN_SET_DIRTY();
}

void Scene::createObject(const char *name, ObjectType type, TreeNode<SceneEntity> &parent) {

    switch(type) {
    case ObjectType::Sphere:
    {
        std::unique_ptr<Sphere> ptr = std::make_unique<Sphere>(generateId(), name);
        parent.createChild(*ptr);
        sceneEntities.emplace_back(std::move(ptr));
        break;
    }
    case ObjectType::Box:
    {
        std::unique_ptr<Box> ptr = std::make_unique<Box>(generateId(), name);
        parent.createChild(*ptr);
        sceneEntities.emplace_back(std::move(ptr));
        break;
    }
    default:
        GO_ASSERT_ALWAYS()
        break;
    }
    
    GEN_SET_DIRTY();
}

void Scene::deleteCsgTreeNode(TreeNode<SceneEntity> &toDelete) {
    pendingDeleteCsgTreeNode.emplace_back(&toDelete);
}

void Scene::moveCsgTreeNode(TreeNode<SceneEntity> &toMove, TreeNode<SceneEntity> &destination) {
    if(csgTreeRoot->moveNode(toMove, destination))
        GEN_SET_DIRTY();
}

void Scene::deleteCsgTreeNodeChildren(TreeNode<SceneEntity> &toDeleteChildren) {
    for(uint32 i = 0; i < toDeleteChildren.getChildCount(); ++i) {
        pendingDeleteCsgTreeNode.emplace_back(&toDeleteChildren.getChildByIndex(i));
    }
}



void Scene::createRootTransform(const char *name, TransformType type) {
    auto transformPtr = internalCreateTransform(name, type);
    transformTreeRoots.emplace_back(std::make_unique<TreeNode<Transform>>(*transformPtr));
    sceneEntities.emplace_back(std::move(transformPtr));
    GEN_SET_DIRTY();
}

void Scene::deleteRootTransform(uint32 treeIndex) {
    GO_ASSERT(treeIndex < transformTreeRoots.size());
    pendingDeleteTransformTreeNode.emplace_back(treeIndex, nullptr);
}

void Scene::createTransform(const char *name, TransformType type, TreeNode<Transform> &parent) {
    auto transformPtr = internalCreateTransform(name, type);
    parent.createChild(*transformPtr);
    sceneEntities.emplace_back(std::move(transformPtr));
    GEN_SET_DIRTY();
}

void Scene::deleteTransformTreeNode(uint32 treeIndex, TreeNode<Transform> &toDelete) {
    GO_ASSERT(treeIndex < transformTreeRoots.size());
    pendingDeleteTransformTreeNode.emplace_back(treeIndex, &toDelete);
}

void Scene::moveTransformTreeNode(uint32 toMoveTreeIndex, TreeNode<Transform> &toMove, uint32 destinationTreeIndex, TreeNode<Transform> &destination) {
    GO_ASSERT(toMoveTreeIndex < transformTreeRoots.size());

    if(transformTreeRoots[toMoveTreeIndex]->moveNode(toMove, destination))
        GEN_SET_DIRTY();
}

void Scene::attachObjectToTransformTreeNode(Object &object, TreeNode<Transform> &transform) {
    object.attachToTransform(*transform);
    GEN_SET_DIRTY();
}

void Scene::detachObjectToTransformTreeNode(Object &object) {
    object.detachFromTransform();
    GEN_SET_DIRTY();
}


void Scene::deleteTransformTreeNodeChildren(uint32 treeIndex, TreeNode<Transform> &toDeleteChildren) {
    for(uint32 i = 0; i < toDeleteChildren.getChildCount(); ++i) {
        pendingDeleteTransformTreeNode.emplace_back(treeIndex, &toDeleteChildren.getChildByIndex(i));
    }
}

std::unique_ptr<Transform> Scene::internalCreateTransform(const char *name, TransformType type) {
    switch(type) {
    case TransformType::Translation:
        return std::make_unique<Translation>(generateId(), name);
    case TransformType::Rotation:
        return std::make_unique<Rotation>(generateId(), name);
    case TransformType::Custom:
        return std::make_unique<CustomTransform>(generateId(), name);
    default:
        GO_ASSERT_ALWAYS()
        return std::unique_ptr<Transform>();
        break;
    }
}


SceneEntity* Scene::getSelectedEntity() { 
    SceneEntity *result = nullptr;

    if(selectedEntityId != -1) {
        auto it = getIteratorToSceneEntityId(selectedEntityId);
        if(it == sceneEntities.end())
            clearSelectedEntity();
        else
            result = it->get();
    }

    return result;
}

SceneEntity* Scene::findSceneEntity(uint32 id) {
    auto it = getIteratorToSceneEntityId(id);
    if(it != sceneEntities.end())
        return it->get();
    else
        return nullptr;
}

typename std::vector<std::unique_ptr<SceneEntity>>::iterator Scene::getIteratorToSceneEntityId(uint32 id) {
    for(auto it = sceneEntities.begin(); it != sceneEntities.end(); ++it) {
        if((*it)->getId() == id) {
            return it;
        }
    }
    return sceneEntities.end();
}

bool Scene::deleteSceneEntity(SceneEntity &toDelete) {
    auto it = getIteratorToSceneEntityId(toDelete.getId());
    if(it != sceneEntities.end()) {
        sceneEntities.erase(it);
        return true;
    }
    return false;
}

void Scene::doPendingOperations() {
    bool deleted = !pendingDeleteCsgTreeNode.empty() || !pendingDeleteTransformTreeNode.empty();

    for(auto toDeleteNodePtr : pendingDeleteCsgTreeNode) {
        toDeleteNodePtr->traverseBfs([this, &toDeleteNodePtr](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {           
            //Delete all children SceneEntities on the tree which toDeleteNode is the toDeleteNode
            deleteSceneEntity(*op);
            return false;
        });

        auto parentOfToDeleteNode = csgTreeRoot->findNodeParent(*toDeleteNodePtr);
        if(parentOfToDeleteNode)
            parentOfToDeleteNode->deleteChild(*toDeleteNodePtr);
    }

    for(auto &pair : pendingDeleteTransformTreeNode) {
        TreeNode<Transform> &toDeleteNode = pair.second ? (*pair.second) : *transformTreeRoots[pair.first];

        toDeleteNode.traverseBfs([this, &pair](TreeNode<Transform> &trans, TreeNode<Transform> *parent) -> bool {
            deleteSceneEntity(*trans);
            return false;
        });

        auto parentOfToDeleteNode = transformTreeRoots[pair.first]->findNodeParent(toDeleteNode);
        if(parentOfToDeleteNode)
            parentOfToDeleteNode->deleteChild(*pair.second);
        else
            transformTreeRoots.erase(transformTreeRoots.begin() + pair.first);
    }

    pendingDeleteCsgTreeNode.clear();
    pendingDeleteTransformTreeNode.clear();

    if(deleted)
        GEN_SET_DIRTY();
}