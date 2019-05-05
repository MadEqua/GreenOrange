#include "CsgOperator.h"


const CsgOperator::TypeInfo CSG_OPERATOR_LIST[] = {
    { CsgOperator::Type::Union, "Union" },
    { CsgOperator::Type::Intersection, "Intersection" },
    { CsgOperator::Type::Subtraction, "Subtraction" },
};

const uint32 CSG_OPERATOR_LIST_SIZE = sizeof(CSG_OPERATOR_LIST) / sizeof(*CSG_OPERATOR_LIST);


CsgOperator::CsgOperator(const char *name, const TypeInfo &typeInfo) :
    name(name),
    typeInfo(typeInfo) {

    objects.push_back(std::make_unique<Object>("o1"));
    objects.push_back(std::make_unique<Object>("o2"));
    objects.push_back(std::make_unique<Object>("o3"));
}

Object& CsgOperator::getObject(uint32 idx) {
    GO_ASSERT(idx < objects.size()); 
    return *objects[idx]; 
}

CsgOperator& CsgOperator::getOperator(uint32 idx) {
    GO_ASSERT(idx < operators.size());
    return *operators[idx]; 
}

void CsgOperator::createOperator(const char *name, const TypeInfo &type) {
    operators.push_back(std::make_unique<CsgOperator>(name, type));
}