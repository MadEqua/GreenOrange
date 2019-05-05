#include "CsgOperator.h"

#include "GreenOrange.h"

CsgOperator::CsgOperator(const char *name, const CsgType type) :
    name(name),
    type(type) {

    objects.emplace_back("o1");
    objects.emplace_back("o2");
    objects.emplace_back("o3");
}

Object& CsgOperator::getObject(uint32 idx) {
    GO_ASSERT(idx < objects.size()); 
    return objects[idx]; 
}

CsgOperator& CsgOperator::getOperator(uint32 idx) {
    GO_ASSERT(idx < operators.size());
    return operators[idx]; 
}

void CsgOperator::createChildOperator(const char *name, CsgType type) {
    operators.emplace_back(name, type);
}