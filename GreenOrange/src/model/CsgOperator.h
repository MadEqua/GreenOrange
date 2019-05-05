#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../Types.h"
#include "../Assert.h"
#include "Object.h"

class CsgOperator
{
public:
    enum class Type {
        Union,
        Intersection,
        Subtraction
    };

    struct TypeInfo {
        Type type;
        std::string name;
    };

    explicit CsgOperator(const char *name, const TypeInfo &typeInfo);

    size_t getObjectCount() const { return objects.size(); }
    bool hasObjects() const { return !objects.empty(); }
    Object& getObject(uint32 idx);

    size_t getOperatorCount() const { return operators.size(); }
    bool hasOperators() const { return !operators.empty(); }
    CsgOperator& getOperator(uint32 idx);
    void createOperator(const char *name, const TypeInfo &type);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

    const TypeInfo& getTypeInfo() const { return typeInfo; }

protected:
    std::string name;
    TypeInfo typeInfo;

    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<CsgOperator>> operators;
};


extern const CsgOperator::TypeInfo CSG_OPERATOR_LIST[];
extern const uint32 CSG_OPERATOR_LIST_SIZE;