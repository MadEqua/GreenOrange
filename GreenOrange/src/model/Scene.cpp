#include "Scene.h"

#include "SceneEntity.h"
#include "Object.h"
#include "Transform.h"


Scene::Scene(const char* name) :
    name(name) {

    sceneEntities.emplace_back(std::make_unique<CsgOperator>(generateId(), "Root Union", CsgType::Union));
    csgTreeRoot = std::make_unique<TreeNode<SceneEntity>>(*sceneEntities[0]);

    sceneEntities.emplace_back(internalCreateTransform("Root Transform", TransformType::Translation));
    transformTreeDummyRoot = std::make_unique<TreeNode<Transform>>(static_cast<Transform&>(*sceneEntities[1]));

    createObject("Default Sphere", ObjectType::Sphere, *csgTreeRoot);
    createLight("Default Light", LightType::Directional);
}

void Scene::createCsgOperator(const char *name, CsgType type, TreeNode<SceneEntity> &parent) {
    std::unique_ptr<CsgOperator> ptr = std::make_unique<CsgOperator>(generateId(), name, type);
    parent.createChild(*ptr);
    sceneEntities.emplace_back(std::move(ptr));
    GEN_SET_DIRTY();
}

void Scene::createObject(const char *name, ObjectType type, TreeNode<SceneEntity> &parent) {
    auto objectPtr = internalCreateObject(name, type);
    parent.createChild(*objectPtr);
    sceneEntities.emplace_back(std::move(objectPtr));
    GEN_SET_DIRTY();
}

void Scene::deleteCsgTreeNode(TreeNode<SceneEntity> &toDelete) {
    pendingDeleteCsgTreeNodes.emplace_back(&toDelete);
}

void Scene::moveCsgTreeNode(TreeNode<SceneEntity> &toMove, TreeNode<SceneEntity> &destination) {
    if(csgTreeRoot->moveNode(toMove, destination))
        GEN_SET_DIRTY();
}

void Scene::swapCsgTreeSiblingNodes(TreeNode<SceneEntity> &node1, TreeNode<SceneEntity> &node2) {
    if(csgTreeRoot->swapSiblingNodes(node1, node2))
        GEN_SET_DIRTY();
}

void Scene::deleteCsgTreeNodeChildren(TreeNode<SceneEntity> &toDeleteChildren) {
    for(uint32 i = 0; i < toDeleteChildren.getChildCount(); ++i) {
        pendingDeleteCsgTreeNodes.emplace_back(&toDeleteChildren.getChildByIndex(i));
    }
}



void Scene::createRootTransform(const char *name, TransformType type) {
    createTransform(name, type, *transformTreeDummyRoot);
}

void Scene::createTransform(const char *name, TransformType type, TreeNode<Transform> &parent) {
    auto transformPtr = internalCreateTransform(name, type);
    parent.createChild(*transformPtr);
    sceneEntities.emplace_back(std::move(transformPtr));
    GEN_SET_DIRTY();
}

void Scene::deleteTransformTreeNode(uint32 treeIndex, TreeNode<Transform> &toDelete) {
    GO_ASSERT(treeIndex < transformTreeDummyRoot->getChildCount());
    pendingDeleteTransformTreeNodes.emplace_back(treeIndex, &toDelete);
}

void Scene::moveTransformTreeNode(uint32 toMoveTreeIndex, TreeNode<Transform> &toMove, TreeNode<Transform> &destination) {
    GO_ASSERT(toMoveTreeIndex < transformTreeDummyRoot->getChildCount());

    if(transformTreeDummyRoot->moveNode(toMove, destination))
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
        pendingDeleteTransformTreeNodes.emplace_back(treeIndex, &toDeleteChildren.getChildByIndex(i));
    }
}

void Scene::createLight(const char *name, LightType type) {
    auto lightPtr = internalCreateLight(name, type);
    lights.emplace_back(lightPtr.get());
    sceneEntities.emplace_back(std::move(lightPtr));
    GEN_SET_DIRTY();
}

void Scene::deleteLight(Light &light) {
    pendingDeleteLights.emplace_back(&light);
}

uint32 Scene::getDirLightCount() const {
    uint32 sum = 0;
    for(const Light *l : lights) {
        if(l->isDirectional()) sum++;
    }
    return sum;
}

uint32 Scene::getPointLightCount() const {
    uint32 sum = 0;
    for(const Light *l : lights) {
        if(l->isPoint()) sum++;
    }
    return sum;
}

std::unique_ptr<Object> Scene::internalCreateObject(const char *name, ObjectType type) {
    switch(type) {
    case ObjectType::Sphere:
        return std::make_unique<Sphere>(generateId(), name);
    case ObjectType::Box:
        return std::make_unique<Box>(generateId(), name);
    default:
        GO_ASSERT_ALWAYS()
        return std::unique_ptr<Object>();
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
    }
}

std::unique_ptr<Light> Scene::internalCreateLight(const char *name, LightType type) {
    switch(type) {
    case LightType::Directional:
        return std::make_unique<DirectionalLight>(generateId(), name);
    case LightType::Point:
        return std::make_unique<PointLight>(generateId(), name);
    default:
        GO_ASSERT_ALWAYS()
        return std::unique_ptr<Light>();
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
    bool deleted = !pendingDeleteCsgTreeNodes.empty() ||
        !pendingDeleteTransformTreeNodes.empty() ||
        !pendingDeleteLights.empty();

    for(auto toDeleteNodePtr : pendingDeleteCsgTreeNodes) {
        auto parentOfToDeleteNode = csgTreeRoot->findNodeParent(*toDeleteNodePtr);

        toDeleteNodePtr->traverseBfs([this, &toDeleteNodePtr](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
            //Delete all children SceneEntities on the tree which toDeleteNode is the toDeleteNode
            deleteSceneEntity(*op);
            return false;
        });

        if(parentOfToDeleteNode)
            parentOfToDeleteNode->deleteChild(*toDeleteNodePtr);
    }

    for(auto &pair : pendingDeleteTransformTreeNodes) {
        auto &toDeleteNode = *pair.second;

        auto parentOfToDeleteNode = transformTreeDummyRoot->findNodeParent(toDeleteNode);

        toDeleteNode.traverseBfs([this, &pair](TreeNode<Transform> &trans, TreeNode<Transform> *parent) -> bool {
            deleteSceneEntity(*trans);
            return false;
        });

        if(parentOfToDeleteNode)
            parentOfToDeleteNode->deleteChild(*pair.second);
    }

    for(auto *light : pendingDeleteLights) {
        deleteSceneEntity(*light);
        auto it = std::find(lights.begin(), lights.end(), light);
        if(it != lights.end())
            lights.erase(it);
    }

    pendingDeleteCsgTreeNodes.clear();
    pendingDeleteTransformTreeNodes.clear();
    pendingDeleteLights.clear();

    if(deleted)
        GEN_SET_DIRTY();
}