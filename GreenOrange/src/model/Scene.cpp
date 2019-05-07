#include "Scene.h"

#include <queue>

#include "CsgOperator.h"
#include "DataRepo.h"


Scene::Scene(const char* name) :
    name(name),
    unionOperator(0, "Union", CsgType::Union),
    nextId(1) {

    //TODO: remove
    unionOperator.createChildOperator(generateId(), "Intersection", CsgType::Intersection);
    unionOperator.createChildOperator(generateId(), "Subtraction", CsgType::Subtraction);
}

void Scene::createCsgOperator(const char *name, CsgType type, CsgOperator &parent) {
    parent.createChildOperator(generateId(), name, type);
}

void Scene::deleteCsgOperator(CsgOperator &toDelete) {
    if(toDelete == unionOperator) return;

    traverseCsgOperatorTree(unionOperator, [&toDelete](CsgOperator &op, CsgOperator *parent) -> bool {
        if(parent && op == toDelete) {
            parent->deleteChildOperator(op);
            return true;
        }
        return false;
    });
}

void Scene::moveCsgOperator(CsgOperator &opToMove, CsgOperator &destination) {
    if(isCsgOperatorDescendentOf(destination, opToMove))
        return;

    traverseCsgOperatorTree(unionOperator, [&opToMove, &destination](CsgOperator &op, CsgOperator *parent) -> bool {
        if(parent && op == opToMove && *parent != destination) {
            destination.moveChildOperator(*parent, opToMove);
            return true;
        }
        return false;
    });
}

bool Scene::isCsgOperatorDescendentOf(CsgOperator &op1, CsgOperator &op2) {
    bool result = false;
    traverseCsgOperatorTree(op2, [&op1, &result](CsgOperator &op, CsgOperator *parent) -> bool {
        if(op == op1) {
            result = true;
            return true;
        }
        return false;
    });
    return result;
}

void Scene::traverseCsgOperatorTree(CsgOperator &root, const std::function<bool(CsgOperator&, CsgOperator*)> &visitFunction) {
    std::queue<CsgOperator*> queue;
    queue.push(&root);

    if(visitFunction(root, nullptr))
        return;

    while(!queue.empty()) {
        CsgOperator &current = *queue.front();
        queue.pop();

        for(uint32 i = 0; i < current.getChildOperatorCount(); ++i) {
            auto &op = current.getChildOperatorByIndex(i);
            if(visitFunction(op, &current))
                return;
            queue.push(&op);
        }
    }
}
