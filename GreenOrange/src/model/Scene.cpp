#include "Scene.h"

#include <queue>
#include <stack>

#include "Object.h"


Scene::Scene(const char* name) :
    name(name),
    unionOperator(0, "Root Union", CsgType::Union),
    nextId(1) {
}

void Scene::createCsgOperator(const char *name, CsgType type, CsgOperator &parent) {
    parent.createChildOperator(generateId(), name, type);
    GEN_SET_DIRTY()
}

void Scene::deleteCsgOperator(CsgOperator &toDelete) {
    if(toDelete == unionOperator) return;

    traverseTreeBfs(unionOperator, [this, &toDelete](SceneEntity &op, CsgOperator *parent) -> bool {
        if(parent && op == toDelete) {
            if(hasSelectedEntity() && op.getId() == selectedEntity->getId())
                clearSelectedEntity();
            parent->deleteChildOperator(dynamic_cast<CsgOperator&>(op));
            GEN_SET_DIRTY()
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
            GEN_SET_DIRTY()
            return true;
        }
        return false;
    });
}

void Scene::createObject(const char *name, ObjectType type, CsgOperator &parent) {
    parent.createChildObject(generateId(), name, type);
    GEN_SET_DIRTY()
}

void Scene::deleteObject(Object &toDelete) {
    traverseTreeBfs(unionOperator, [this, &toDelete](SceneEntity &op, CsgOperator *parent) -> bool {
        if(parent && op == toDelete) {
            if(hasSelectedEntity() && op.getId() == selectedEntity->getId())
                clearSelectedEntity();
            parent->deleteChildObject(dynamic_cast<Object&>(op));
            GEN_SET_DIRTY()
            return true;
        }
        return false;
    });
}

void Scene::moveObject(Object &opToMove, CsgOperator &destination) {
    traverseTreeBfs(unionOperator, [&opToMove, &destination](SceneEntity &op, CsgOperator *parent) -> bool {
        if(parent && op == opToMove && *parent != destination) {
            destination.moveChildObject(*parent, opToMove);
            GEN_SET_DIRTY()
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

void Scene::traverseTreeDfs(CsgOperator &root, const std::function<bool(SceneEntity&)> &visitFunction) {
    std::stack<SceneEntity*> stack;
    stack.push(&root);

    while(!stack.empty()) {
        SceneEntity &current = *stack.top();
        stack.pop();

        if(visitFunction(current))
            return;

        if(current.isCsgOperator()) {
            CsgOperator &currentOp = dynamic_cast<CsgOperator&>(current);

            //Pushing backwards so that it visit child nodes from 0 to N ("left to right").
            for(int i = currentOp.getChildObjectCount() - 1; i >= 0; --i) {
                auto &obj = currentOp.getChildObjectByIndex(i);
                stack.push(&obj);
            }

            for(int i = currentOp.getChildOperatorCount() - 1; i >= 0; --i) {
                auto &op = currentOp.getChildOperatorByIndex(i);
                stack.push(&op);
            }
        }
    }
}