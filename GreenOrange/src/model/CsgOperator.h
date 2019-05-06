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
    CsgOperator(uint32 id, const char *name, CsgType type);

    uint32 getId() const { return id; }
    bool isEmpty() const { return childObjects.empty() && childOperators.empty(); }

    size_t getObjectCount() const { return childObjects.size(); }
    bool hasObjects() const { return !childObjects.empty(); }
    Object& getObjectByIndex(uint32 idx);
    void createChildObject(const char *name);

    size_t getOperatorCount() const { return childOperators.size(); }
    bool hasOperators() const { return !childOperators.empty(); }
    CsgOperator& getOperatorByIndex(uint32 idx);
    void createChildOperator(uint32 id, const char *name, CsgType type);
    void addChildOperator(CsgOperator &op);
    void addChildOperator(CsgOperator &&op);
    void deleteChildOperator(CsgOperator &op);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

    CsgType getType() const { return type; }

    bool operator==(const CsgOperator& other) { return id == other.id; }

protected:
    uint32 id;
    std::string name;
    CsgType type;

    std::vector<Object> childObjects;
    std::vector<CsgOperator> childOperators;
};