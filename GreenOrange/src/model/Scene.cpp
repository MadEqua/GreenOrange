#include "Scene.h"

#include "SceneEntity.h"
#include "Object.h"


Scene::Scene(const char* name) :
    name(name),
    treeRoot(CsgOperator(0, "Root Union", CsgType::Union)),
    nextId(1) {
}

void Scene::createCsgOperator(const char *name, CsgType type, TreeNode<SceneEntity> &parent) {
    parent.createChild(CsgOperator(generateId(), name, type));
    GEN_SET_DIRTY()
}

void Scene::createObject(const char *name, ObjectType type, TreeNode<SceneEntity> &parent) {
    switch(type) {
    case ObjectType::Sphere:
        parent.createChild(Sphere(generateId(), name));
        break;
    case ObjectType::Box:
        parent.createChild(Box(generateId(), name));
        break;
    default:
        break;
    }
    GEN_SET_DIRTY()
}

void Scene::deleteNode(TreeNode<SceneEntity> &toDelete) {
    if(treeRoot.deleteNode(toDelete))
        GEN_SET_DIRTY()
}

void Scene::moveNode(TreeNode<SceneEntity> &toMove, TreeNode<SceneEntity> &destination) {
    if(treeRoot.moveNode(toMove, destination))
        GEN_SET_DIRTY()
}

void Scene::deleteNodeChildren(TreeNode<SceneEntity> &toDelete) {
    if(toDelete.deleteChildren())
        GEN_SET_DIRTY()
}

SceneEntity* Scene::getSelectedEntity() { 
    SceneEntity *result = nullptr;

    if(selectedEntityId != -1) {
        treeRoot.traverseBfs([this, &result](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
            if(op->getId() == selectedEntityId) {
                result = &op.getPayload();
                return true;
            }
            return false;
        });

        if(!result)
            clearSelectedEntity();
    }

    return result;
}