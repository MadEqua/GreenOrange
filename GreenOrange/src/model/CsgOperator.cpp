#include "CsgOperator.h"

#include "../Assert.h"
#include "../GlslGenerator.h"


CsgOperator::CsgOperator(uint32 id, const char *name, const CsgType type) :
    SceneEntity(SceneEntityType::CsgOperator, id, name),
    type(type) {

    createChildObject(999 + id, "sphere", ObjectType::Sphere);
}

Object& CsgOperator::getChildObjectByIndex(uint32 idx) {
    GO_ASSERT(idx < childObjects.size()); 
    return *childObjects[idx]; 
}

CsgOperator& CsgOperator::getChildOperatorByIndex(uint32 idx) {
    GO_ASSERT(idx < childOperators.size());
    return *childOperators[idx]; 
}

void CsgOperator::createChildObject(uint32 id, const char *name, ObjectType type) {
    switch(type)
    {
    case ObjectType::Sphere:
        childObjects.emplace_back(std::make_unique<Sphere>(id, name));
        break;
    case ObjectType::Box:
        childObjects.emplace_back(std::make_unique<Box>(id, name));
        break;
    default:
        break;
    }

    GEN_SET_DIRTY()
}

void CsgOperator::deleteChildObject(Object &object) {
    auto it = findChildObject(object);
    if(it != childObjects.end()) {
        childObjects.erase(it);
        GEN_SET_DIRTY()
    }
}

void CsgOperator::clearChildObjects() {
    childObjects.clear();
    GEN_SET_DIRTY()
}


void CsgOperator::moveChildObject(CsgOperator &parentToMoveFrom, Object&toMove) {
    auto toMoveIt = parentToMoveFrom.findChildObject(toMove);
    if(toMoveIt != parentToMoveFrom.childObjects.end()) {
        std::move(toMoveIt, toMoveIt + 1, std::back_inserter(childObjects));
        parentToMoveFrom.childObjects.erase(toMoveIt);
        GEN_SET_DIRTY()
    }
}

void CsgOperator::createChildOperator(uint32 id, const char *name, CsgType type) {
    childOperators.emplace_back(std::make_unique<CsgOperator>(id, name, type));
    GEN_SET_DIRTY()
}

void CsgOperator::moveChildOperator(CsgOperator &parentToMoveFrom, CsgOperator &toMove) {
    auto toMoveIt = parentToMoveFrom.findChildOperator(toMove);
    if(toMoveIt != parentToMoveFrom.childOperators.end()) {
        std::move(toMoveIt, toMoveIt + 1, std::back_inserter(childOperators));
        parentToMoveFrom.childOperators.erase(toMoveIt);
        GEN_SET_DIRTY()
    }
}

void CsgOperator::deleteChildOperator(CsgOperator &op) {
    auto it = findChildOperator(op);
    if(it != childOperators.end()) {
        childOperators.erase(it);
        GEN_SET_DIRTY()
    }
}

void CsgOperator::clearChildOperators() {
    childOperators.clear();
    GEN_SET_DIRTY()
}

void CsgOperator::deleteChildren() {
    childObjects.clear();
    childOperators.clear();
    GEN_SET_DIRTY()
}

std::vector<std::unique_ptr<Object>>::iterator CsgOperator::findChildObject(const Object &op) {
    for(auto it = childObjects.begin(); it != childObjects.end(); ++it) {
        if((*it)->getId() == op.getId()) {
            return it;
        }
    }
    return childObjects.end();
}

std::vector<std::unique_ptr<CsgOperator>>::iterator CsgOperator::findChildOperator(const CsgOperator &op) {
    for(auto it = childOperators.begin(); it != childOperators.end(); ++it) {
        if((*it)->getId() == op.getId()) {
            return it;
        }
    }
    return childOperators.end();
}