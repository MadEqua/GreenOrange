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
    toDelete.traverseBfs([this](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
        deleteSceneEntity(*op);
        return false;
    });

    if(csgTreeRoot->deleteNode(toDelete)) {
        GEN_SET_DIRTY();
    }
}

void Scene::moveCsgTreeNode(TreeNode<SceneEntity> &toMove, TreeNode<SceneEntity> &destination) {
    if(csgTreeRoot->moveNode(toMove, destination))
        GEN_SET_DIRTY();
}

void Scene::deleteCsgTreeNodeChildren(TreeNode<SceneEntity> &toDeleteChildren) {
    toDeleteChildren.traverseBfs([this, &toDeleteChildren](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
        if(*op != *toDeleteChildren) {
            deleteSceneEntity(*op);
        }
        return false;
    });

    if(toDeleteChildren.deleteChildren()) {
        GEN_SET_DIRTY();
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
    
    (*transformTreeRoots[treeIndex]).traverseBfs([this](TreeNode<Transform> &op, TreeNode<Transform> *parent) -> bool {
        deleteSceneEntity(*op);
        return false;
    });

    transformTreeRoots.erase(transformTreeRoots.begin() + treeIndex);
    GEN_SET_DIRTY();
}

void Scene::createTransform(const char *name, TransformType type, TreeNode<Transform> &parent) {
    auto transformPtr = internalCreateTransform(name, type);
    parent.createChild(*transformPtr);
    sceneEntities.emplace_back(std::move(transformPtr));
    GEN_SET_DIRTY();
}

void Scene::deleteTransformTreeNode(uint32 treeIndex, TreeNode<Transform> &toDelete) {
    GO_ASSERT(treeIndex < transformTreeRoots.size());

    toDelete.traverseBfs([this](TreeNode<Transform> &op, TreeNode<Transform> *parent) -> bool {
        deleteSceneEntity(*op);
        return false;
    });

    if(transformTreeRoots[treeIndex]->deleteNode(toDelete)) {
        GEN_SET_DIRTY();
    }
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


void Scene::deleteTransformTreeNodeChildren(TreeNode<Transform> &toDeleteChildren) {
    toDeleteChildren.traverseBfs([this, &toDeleteChildren](TreeNode<Transform> &op, TreeNode<Transform> *parent) -> bool {
        if(*op != *toDeleteChildren) {
            deleteSceneEntity(*op);
        }
        return false;
    });

    if(toDeleteChildren.deleteChildren()) {
        GEN_SET_DIRTY();
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