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
    deleteTreeNodeChildren(toDeleteChildren);
}



void Scene::createRootTransform(const char *name, TransformType type) {
    auto transformPtr = internalCreateTransform(name, type);
    transformTreeRoots.emplace_back(std::make_unique<TreeNode<SceneEntity>>(*transformPtr));
    sceneEntities.emplace_back(std::move(transformPtr));
    GEN_SET_DIRTY();
}

void Scene::deleteRootTransform(uint32 treeIndex) {
    GO_ASSERT(treeIndex < transformTreeRoots.size());
    
    (*transformTreeRoots[treeIndex]).traverseBfs([this](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
        deleteSceneEntity(*op);
        return false;
    });

    transformTreeRoots.erase(transformTreeRoots.begin() + treeIndex);
    GEN_SET_DIRTY();
}

void Scene::createTransform(const char *name, TransformType type, TreeNode<SceneEntity> &parent) {
    auto transformPtr = internalCreateTransform(name, type);
    parent.createChild(*transformPtr);
    sceneEntities.emplace_back(std::move(transformPtr));
    GEN_SET_DIRTY();
}

void Scene::deleteTransformTreeNode(uint32 treeIndex, TreeNode<SceneEntity> &toDelete) {
    GO_ASSERT(treeIndex < transformTreeRoots.size());

    toDelete.traverseBfs([this](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
        deleteSceneEntity(*op);
        return false;
    });

    if(transformTreeRoots[treeIndex]->deleteNode(toDelete)) {
        GEN_SET_DIRTY();
    }
}

void Scene::moveTransformTreeNode(uint32 toMoveTreeIndex, TreeNode<SceneEntity> &toMove, uint32 destinationTreeIndex, TreeNode<SceneEntity> &destination) {
    GO_ASSERT(toMoveTreeIndex < transformTreeRoots.size());

    if(transformTreeRoots[toMoveTreeIndex]->moveNode(toMove, destination))
        GEN_SET_DIRTY();
}

void Scene::attachObjectToTransformTreeNode(TreeNode<SceneEntity> &object, TreeNode<SceneEntity> &transform) {
    if(!transform.findNode(object)) {
        transform.createChild(*object);
        GEN_SET_DIRTY();
    }
}

void Scene::deleteTransformTreeNodeChildren(TreeNode<SceneEntity> &toDeleteChildren) {
    deleteTreeNodeChildren(toDeleteChildren);
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



void Scene::deleteTreeNodeChildren(TreeNode<SceneEntity> &toDeleteChildren) {
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

SceneEntity* Scene::getSelectedEntity() { 
    SceneEntity *result = nullptr;

    if(selectedEntityId != -1) {
        auto it = findSceneEntityId(selectedEntityId);
        if(it == sceneEntities.end())
            clearSelectedEntity();
        else
            result = it->get();
    }

    return result;
}

typename std::vector<std::unique_ptr<SceneEntity>>::iterator Scene::findSceneEntityId(uint32 id) {
    for(auto it = sceneEntities.begin(); it != sceneEntities.end(); ++it) {
        if((*it)->getId() == id) {
            return it;
        }
    }
    return sceneEntities.end();
}

bool Scene::deleteSceneEntity(SceneEntity &toDelete) {
    auto it = findSceneEntityId(toDelete.getId());
    if(it != sceneEntities.end()) {
        sceneEntities.erase(it);
        return true;
    }
    return false;
}