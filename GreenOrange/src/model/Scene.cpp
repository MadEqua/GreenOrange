#include "Scene.h"

#include <queue>
#include <stack>

#include "CsgOperator.h"
#include "DataRepo.h"


Scene::Scene(const char* name) :
    name(name),
    unionOperator(0, "Union", CsgType::Union),
    nextId(1) {

    //TODO: remove
    createCsgOperator("Union", CsgType::Union, unionOperator);
    //createCsgOperator("Subtraction", CsgType::Subtraction, unionOperator);
}

void Scene::createCsgOperator(const char *name, CsgType type, CsgOperator &parent) {
    parent.createChildOperator(generateId(), name, type);
}

void Scene::deleteCsgOperator(CsgOperator &toDelete) {
    if(toDelete == unionOperator) return;

    traverseTreeBfs(unionOperator, [this, &toDelete](SceneEntity &op, CsgOperator *parent) -> bool {
        if(parent && op == toDelete) {
            if(hasSelectedEntity() && op.getId() == selectedEntity->getId())
                clearSelectedEntity();
            parent->deleteChildOperator(dynamic_cast<CsgOperator&>(op));
            return true;
        }
        return false;
    });
}

void Scene::moveCsgOperator(CsgOperator &toMove, CsgOperator &destination) {
    if(isCsgOperatorDescendentOf(destination, toMove))
        return;

    traverseTreeBfs(unionOperator, [&toMove, &destination](SceneEntity &op, CsgOperator *parent) -> bool {
        if(parent && op == toMove && *parent != destination) {
            destination.moveChildOperator(*parent, toMove);
            return true;
        }
        return false;
    });
}

void Scene::createObject(const char *name, ObjectType type, CsgOperator &parent) {
    parent.createChildObject(generateId(), name, type);
}

void Scene::deleteObject(Object &toDelete) {
    traverseTreeBfs(unionOperator, [this, &toDelete](SceneEntity &op, CsgOperator *parent) -> bool {
        if(parent && op == toDelete) {
            if(hasSelectedEntity() && op.getId() == selectedEntity->getId())
                clearSelectedEntity();
            parent->deleteChildObject(dynamic_cast<Object&>(op));
            return true;
        }
        return false;
    });
}

void Scene::moveObject(Object &opToMove, CsgOperator &destination) {
    traverseTreeBfs(unionOperator, [&opToMove, &destination](SceneEntity &op, CsgOperator *parent) -> bool {
        if(parent && op == opToMove && *parent != destination) {
            destination.moveChildObject(*parent, opToMove);
            return true;
        }
        return false;
    });
}

bool Scene::isCsgOperatorDescendentOf(CsgOperator &op1, CsgOperator &op2) {
    bool result = false;
    traverseTreeBfs(op2, [&op1, &result](SceneEntity &op, CsgOperator *parent) -> bool {
        if(op == op1) {
            result = true;
            return true;
        }
        return false;
    });
    return result;
}

void Scene::traverseTreeBfs(CsgOperator &root, const std::function<bool(SceneEntity&, CsgOperator*)> &visitFunction) {
    std::queue<CsgOperator*> queue;
    queue.push(&root);

    if(visitFunction(root, nullptr))
        return;

    while(!queue.empty()) {
        CsgOperator &current = *queue.front();
        queue.pop();

        for(uint32 i = 0; i < current.getChildObjectCount(); ++i) {
            auto &op = current.getChildObjectByIndex(i);
            if(visitFunction(op, &current))
                return;
        }

        for(uint32 i = 0; i < current.getChildOperatorCount(); ++i) {
            auto &op = current.getChildOperatorByIndex(i);
            if(visitFunction(op, &current))
                return;
            queue.push(&op);
        }
    }
}

void Scene::traverseTreeDfs(CsgOperator &root, const std::function<bool(SceneEntity&, CsgOperator*)> &visitFunction) {
    std::stack<CsgOperator*> stack;
    stack.push(&root);

    if(visitFunction(root, nullptr))
        return;

    while(!stack.empty()) {
        CsgOperator &current = *stack.top();
        stack.pop();

        for(uint32 i = 0; i < current.getChildObjectCount(); ++i) {
            auto &op = current.getChildObjectByIndex(i);
            if(visitFunction(op, &current))
                return;
        }

        for(uint32 i = 0; i < current.getChildOperatorCount(); ++i) {
            auto &op = current.getChildOperatorByIndex(i);
            if(visitFunction(op, &current))
                return;
            stack.push(&op);
        }
    }
}