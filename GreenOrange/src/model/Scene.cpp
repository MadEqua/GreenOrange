#include "Scene.h"

#include <queue>

#include "CsgOperator.h"
#include "DataRepo.h"


Scene::Scene(const char* name) :
    name(name),
    unionOperator(0, "Union", CsgType::Union),
    nextId(1) {
}

void Scene::createCsgOperator(const char *name, CsgType type, CsgOperator &parent) {
    parent.createChildOperator(generateId(), name, type);
}

void Scene::deleteCsgOperator(CsgOperator &toDelete) {
    if(toDelete == unionOperator) return;

    std::queue<CsgOperator*> queue;
    queue.push(&unionOperator);

    while(!queue.empty()) {
        CsgOperator &current = *queue.front();
        queue.pop();

        for(uint32 i = 0; i < current.getOperatorCount(); ++i) {
            CsgOperator &child = current.getOperatorByIndex(i);
            if(child == toDelete) {
                current.deleteChildOperator(toDelete);
                return;
            }
            queue.push(&child);
        }
    }
}

void Scene::moveCsgOperator(CsgOperator &opToMove, CsgOperator &destination) {
    std::queue<CsgOperator*> queue;
    queue.push(&unionOperator);

    CsgOperator *parent = nullptr;

    while(!queue.empty()) {
        CsgOperator &current = *queue.front();
        queue.pop();

        for(uint32 i = 0; i < current.getOperatorCount(); ++i) {
            CsgOperator &child = current.getOperatorByIndex(i);
            if(child == opToMove) {
                parent = &current;
                break;
            }
            queue.push(&child);
        }

        if(parent) {
            destination.addChildOperator(std::move(opToMove));
            parent->deleteChildOperator(opToMove);
        }
    }
}