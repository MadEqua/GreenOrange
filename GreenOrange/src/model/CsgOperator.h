#pragma once

#include <string>
#include <vector>

#include "../Types.h"
#include "../Assert.h"
#include "DataRepo.h"
#include "Object.h"


class CsgOperator
{
public:
    CsgOperator(const char *name, CsgType type);

    size_t getObjectCount() const { return objects.size(); }
    bool hasObjects() const { return !objects.empty(); }
    Object& getObject(uint32 idx);

    size_t getOperatorCount() const { return operators.size(); }
    bool hasOperators() const { return !operators.empty(); }
    CsgOperator& getOperator(uint32 idx);
    void createChildOperator(const char *name, CsgType type);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

    CsgType getTypeInfo() const { return type; }

protected:
    std::string name;
    CsgType type;

    std::vector<Object> objects;
    std::vector<CsgOperator> operators;
};