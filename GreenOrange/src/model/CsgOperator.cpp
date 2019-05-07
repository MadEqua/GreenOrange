#include "CsgOperator.h"

#include <algorithm>
#include "GreenOrange.h"

CsgOperator::CsgOperator(uint32 id, const char *name, const CsgType type) :
    id(id),
    name(name),
    type(type) {

    /*childObjects.emplace_back("o1");
    childObjects.emplace_back("o2");
    childObjects.emplace_back("o3");*/
}

Object& CsgOperator::getChildObjectByIndex(uint32 idx) {
    GO_ASSERT(idx < childObjects.size()); 
    return *childObjects[idx]; 
}

CsgOperator& CsgOperator::getChildOperatorByIndex(uint32 idx) {
    GO_ASSERT(idx < childOperators.size());
    return *childOperators[idx]; 
}

void CsgOperator::createChildObject(const char *name) {
    childObjects.emplace_back(std::make_unique<Object>(name));
}

void CsgOperator::createChildOperator(uint32 id, const char *name, CsgType type) {
    childOperators.emplace_back(std::make_unique<CsgOperator>(id, name, type));
}

void CsgOperator::moveChildOperator(CsgOperator &parentToMoveFrom, CsgOperator &toMove) {
    auto toMoveIt = parentToMoveFrom.findChildOperatorById(toMove);
    if(toMoveIt != parentToMoveFrom.childOperators.end()) {
        std::move(toMoveIt, toMoveIt + 1, std::back_inserter(childOperators));
        parentToMoveFrom.childOperators.erase(toMoveIt);
    }
}

void CsgOperator::deleteChildOperator(CsgOperator &op) {
    auto it = findChildOperatorById(op);
    if(it != childOperators.end()) {
        childOperators.erase(it);
    }
}

std::vector<std::unique_ptr<CsgOperator>>::iterator CsgOperator::findChildOperatorById(const CsgOperator &op) {
    for(auto it = childOperators.begin(); it != childOperators.end(); ++it) {
        if((*it)->getId() == op.getId()) {
            return it;
        }
    }
    return childOperators.end();
}