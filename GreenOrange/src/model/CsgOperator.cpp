#include "CsgOperator.h"

#include "GreenOrange.h"

CsgOperator::CsgOperator(uint32 id, const char *name, const CsgType type) :
    id(id),
    name(name),
    type(type) {

    childObjects.emplace_back("o1");
    childObjects.emplace_back("o2");
    childObjects.emplace_back("o3");
}

Object& CsgOperator::getObjectByIndex(uint32 idx) {
    GO_ASSERT(idx < childObjects.size()); 
    return childObjects[idx]; 
}

CsgOperator& CsgOperator::getOperatorByIndex(uint32 idx) {
    GO_ASSERT(idx < childOperators.size());
    return childOperators[idx]; 
}

void CsgOperator::createChildObject(const char *name) {
    childObjects.emplace_back(name);
}

void CsgOperator::createChildOperator(uint32 id, const char *name, CsgType type) {
    childOperators.emplace_back(id, name, type);
}

void CsgOperator::addChildOperator(CsgOperator &op) {
    childOperators.push_back(op);
}

void CsgOperator::addChildOperator(CsgOperator &&op) {
    childOperators.push_back(std::move(op));
}

void CsgOperator::deleteChildOperator(CsgOperator &op) {
    childOperators.erase(std::find(childOperators.begin(), childOperators.end(), op));
}