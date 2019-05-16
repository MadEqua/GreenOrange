#include "Scene.h"

#include "SceneEntity.h"
#include "Object.h"
#include "Transform.h"


Scene::Scene(const char* name) :
    name(name),
    csgTreeRoot(CsgOperator(0, "Root Union", CsgType::Union)),
    transformTreeRoot(Translation(1, "Sample Transform")),
    nextId(2) {
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

void Scene::deleteCsgTreeNode(TreeNode<SceneEntity> &toDelete) {
    if(csgTreeRoot.deleteTreeNode(toDelete))
        GEN_SET_DIRTY()
}

void Scene::moveCsgTreeNode(TreeNode<SceneEntity> &toMove, TreeNode<SceneEntity> &destination) {
    if(csgTreeRoot.moveTreeNode(toMove, destination))
        GEN_SET_DIRTY()
}

void Scene::deleteCsgTreeNodeChildren(TreeNode<SceneEntity> &toDelete) {
    if(toDelete.deleteChildren())
        GEN_SET_DIRTY()
}



void Scene::createTransform(const char *name, TransformType type, TreeNode<Transform> &parent) {
    switch(type) {
    case TransformType::Translation:
        parent.createChild(Translation(generateId(), name));
        break;
    case TransformType::Rotation:
        parent.createChild(Rotation(generateId(), name));
        break;
    case TransformType::Custom:
        parent.createChild(CustomTransform(generateId(), name));
        break;
    default:
        break;
    }
    GEN_SET_DIRTY()
}

void Scene::deleteTransformTreeNode(TreeNode<Transform> &toDelete) {
    if(transformTreeRoot.deleteTreeNode(toDelete))
        GEN_SET_DIRTY()
}

void Scene::moveTransformTreeNode(TreeNode<Transform> &toMove, TreeNode<Transform> &destination) {
    if(transformTreeRoot.moveTreeNode(toMove, destination))
        GEN_SET_DIRTY()
}

void Scene::deleteTransformTreeNodeChildren(TreeNode<Transform> &toDelete) {
    if(toDelete.deleteChildren())
        GEN_SET_DIRTY()
}

SceneEntity* Scene::getSelectedEntity() { 
    SceneEntity *result = nullptr;

    if(selectedEntityId != -1) {
        csgTreeRoot.traverseBfs([this, &result](TreeNode<SceneEntity> &op, TreeNode<SceneEntity> *parent) -> bool {
            if(op->getId() == selectedEntityId) {
                result = &op.getPayload();
                return true;
            }
            return false;
        });

        transformTreeRoot.traverseBfs([this, &result](TreeNode<Transform> &tr, TreeNode<Transform> *parent) -> bool {
            if(tr->getId() == selectedEntityId) {
                result = &tr.getPayload();
                return true;
            }
            return false;
        });

        if(!result)
            clearSelectedEntity();
    }

    return result;
}