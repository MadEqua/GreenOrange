#include "Scene.h"

#include "Entity.h"
#include "Object.h"
#include "Transform.h"
#include "Project.h"


Scene::Scene(const char* name) :
    name(name) {

    sceneEntities.emplace_back(std::make_unique<CsgOperator>(Project::generateId(), "Root Union", CsgType::Union));
    csgTreeRoot = std::make_unique<TreeNode<Entity>>(*sceneEntities[0]);

    sceneEntities.emplace_back(internalCreateTransform("Root Transform", TransformType::Translation));
    transformTreeDummyRoot = std::make_unique<TreeNode<Transform>>(static_cast<Transform&>(*sceneEntities[1]));

    createLight("Default Light", LightType::Point);
    createObject("Default Sphere", ObjectType::Sphere, *csgTreeRoot);

    const float size = 15.0f;

    createRootTransform("TransformWall1", TransformType::Translation);
    auto &trans1 = static_cast<Translation&>(*sceneEntities[sceneEntities.size() - 1]);
    trans1.setAmmount(glm::vec3(size, 0.0f, 0.0f));
    
    createRootTransform("TransformWall2", TransformType::Translation);
    auto &trans2 = static_cast<Translation&>(*sceneEntities[sceneEntities.size() - 1]);
    trans2.setAmmount(glm::vec3(-size, 0.0f, 0.0f));
    
    createRootTransform("TransformWall3", TransformType::Translation);
    auto &trans3 = static_cast<Translation&>(*sceneEntities[sceneEntities.size() - 1]);
    trans3.setAmmount(glm::vec3(0.0f, 0.0f, size));
    
    /*createRootTransform("TransformWall4", TransformType::Translation);
    auto &trans4 = static_cast<Translation&>(*sceneEntities[sceneEntities.size() - 1]);
    trans4.setAmmount(glm::vec3(0.0f, size, 0.0f));
    
    createRootTransform("TransformWall5", TransformType::Translation);
    auto &trans5 = static_cast<Translation&>(*sceneEntities[sceneEntities.size() - 1]);
    trans4.setAmmount(glm::vec3(0.0f, -size, 0.0f));*/


    createObject("Wall 1", ObjectType::Box, *csgTreeRoot);
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).setDimensions(glm::vec3(0.1f, size, size));
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).attachToTransform(trans1);
    
    createObject("Wall 2", ObjectType::Box, *csgTreeRoot);
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).setDimensions(glm::vec3(0.2f, size, size));
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).attachToTransform(trans2);

    createObject("Wall 3", ObjectType::Box, *csgTreeRoot);
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).setDimensions(glm::vec3(size, size, 0.2f));
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).attachToTransform(trans3);

    /*createObject("Wall 4", ObjectType::Box, *csgTreeRoot);
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).setDimensions(glm::vec3(size, 0.2f, size));
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).attachToTransform(trans4);

    createObject("Wall 5", ObjectType::Box, *csgTreeRoot);
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).setDimensions(glm::vec3(size, 0.2f, size));
    static_cast<Box&>(*sceneEntities[sceneEntities.size() - 1]).attachToTransform(trans5);*/
}

void Scene::createCsgOperator(const char *name, CsgType type, TreeNode<Entity> &parent) {
    std::unique_ptr<CsgOperator> ptr = std::make_unique<CsgOperator>(Project::generateId(), name, type);
    parent.createChild(*ptr);
    sceneEntities.emplace_back(std::move(ptr));
    PREVIEW_SET_DIRTY();
}

void Scene::createObject(const char *name, ObjectType type, TreeNode<Entity> &parent) {
    auto objectPtr = internalCreateObject(name, type);
    parent.createChild(*objectPtr);
    sceneEntities.emplace_back(std::move(objectPtr));
    PREVIEW_SET_DIRTY();
}

void Scene::deleteCsgTreeNode(TreeNode<Entity> &toDelete) {
    pendingDeleteCsgTreeNodes.emplace_back(&toDelete);
}

void Scene::moveCsgTreeNode(TreeNode<Entity> &toMove, TreeNode<Entity> &destination) {
    if(csgTreeRoot->moveNode(toMove, destination))
        PREVIEW_SET_DIRTY();
}

void Scene::swapCsgTreeSiblingNodes(TreeNode<Entity> &node1, TreeNode<Entity> &node2) {
    if(csgTreeRoot->swapSiblingNodes(node1, node2))
        PREVIEW_SET_DIRTY();
}

void Scene::deleteCsgTreeNodeChildren(TreeNode<Entity> &toDeleteChildren) {
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
    PREVIEW_SET_DIRTY();
}

void Scene::deleteTransformTreeNode(uint32 treeIndex, TreeNode<Transform> &toDelete) {
    GO_ASSERT(treeIndex < transformTreeDummyRoot->getChildCount());
    pendingDeleteTransformTreeNodes.emplace_back(treeIndex, &toDelete);
}

void Scene::moveTransformTreeNode(uint32 toMoveTreeIndex, TreeNode<Transform> &toMove, TreeNode<Transform> &destination) {
    GO_ASSERT(toMoveTreeIndex < transformTreeDummyRoot->getChildCount());

    if(transformTreeDummyRoot->moveNode(toMove, destination))
        PREVIEW_SET_DIRTY();
}

void Scene::attachObjectToTransformTreeNode(Object &object, TreeNode<Transform> &transform) {
    object.attachToTransform(*transform);
    PREVIEW_SET_DIRTY();
}

void Scene::detachObjectToTransformTreeNode(Object &object) {
    object.detachFromTransform();
    PREVIEW_SET_DIRTY();
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
    PREVIEW_SET_DIRTY();
}

void Scene::deleteLight(Light &light) {
    pendingDeleteLights.emplace_back(&light);
}

uint32 Scene::getStaticLightCount() const {
    uint32 sum = 0;
    for(const Light *l : lights) {
        if(l->isStatic()) sum++;
    }
    return sum;
}

uint32 Scene::getDirLightCount() const {
    uint32 sum = 0;
    for(const Light *l : lights) {
        if(l->isDirectional()) sum++;
    }
    return sum;
}

uint32 Scene::getStaticDirLightCount() const {
    uint32 sum = 0;
    for(const Light *l : lights) {
        if(l->isStatic() && l->isDirectional()) sum++;
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

uint32 Scene::getStaticPointLightCount() const {
    uint32 sum = 0;
    for(const Light *l : lights) {
        if(l->isStatic() && l->isPoint()) sum++;
    }
    return sum;
}

void Scene::createProbe(const char *name) {
    auto probePtr = std::make_unique<Probe>(Project::generateId(), name);
    probes.emplace_back(probePtr.get());
    sceneEntities.emplace_back(std::move(probePtr));
    PREVIEW_SET_DIRTY();
}

void Scene::deleteProbe(Probe &probe) {
    pendingDeleteProbes.emplace_back(&probe);
}

std::unique_ptr<Object> Scene::internalCreateObject(const char *name, ObjectType type) {
    switch(type) {
    case ObjectType::Sphere:
        return std::make_unique<Sphere>(Project::generateId(), name);
    case ObjectType::Box:
        return std::make_unique<Box>(Project::generateId(), name);
    default:
        GO_ASSERT_ALWAYS()
        return std::unique_ptr<Object>();
    }
}

std::unique_ptr<Transform> Scene::internalCreateTransform(const char *name, TransformType type) {
    switch(type) {
    case TransformType::Translation:
        return std::make_unique<Translation>(Project::generateId(), name);
    case TransformType::Rotation:
        return std::make_unique<Rotation>(Project::generateId(), name);
    case TransformType::Custom:
        return std::make_unique<CustomTransform>(Project::generateId(), name);
    default:
        GO_ASSERT_ALWAYS()
        return std::unique_ptr<Transform>();
    }
}

std::unique_ptr<Light> Scene::internalCreateLight(const char *name, LightType type) {
    switch(type) {
    case LightType::Directional:
        return std::make_unique<DirectionalLight>(Project::generateId(), name);
    case LightType::Point:
        return std::make_unique<PointLight>(Project::generateId(), name);
    default:
        GO_ASSERT_ALWAYS()
        return std::unique_ptr<Light>();
    }
}


Entity* Scene::findSceneEntity(uint32 id) {
    auto it = getIteratorToSceneEntityId(id);
    if(it != sceneEntities.end())
        return it->get();
    else
        return nullptr;
}

typename std::vector<std::unique_ptr<Entity>>::iterator Scene::getIteratorToSceneEntityId(uint32 id) {
    for(auto it = sceneEntities.begin(); it != sceneEntities.end(); ++it) {
        if((*it)->getId() == id) {
            return it;
        }
    }
    return sceneEntities.end();
}

bool Scene::deleteSceneEntity(Entity &toDelete) {
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
        !pendingDeleteLights.empty() ||
        !pendingDeleteProbes.empty();

    for(auto toDeleteNodePtr : pendingDeleteCsgTreeNodes) {
        auto parentOfToDeleteNode = csgTreeRoot->findNodeParent(*toDeleteNodePtr);

        toDeleteNodePtr->traverseBfs([this, &toDeleteNodePtr](TreeNode<Entity> &op, TreeNode<Entity> *parent) -> bool {
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

    for(auto *probe : pendingDeleteProbes) {
        deleteSceneEntity(*probe);
        auto it = std::find(probes.begin(), probes.end(), probe);
        if(it != probes.end())
            probes.erase(it);
    }

    pendingDeleteCsgTreeNodes.clear();
    pendingDeleteTransformTreeNodes.clear();
    pendingDeleteLights.clear();
    pendingDeleteProbes.clear();

    if(deleted)
        PREVIEW_SET_DIRTY();
}