#pragma once

#include <string>
#include <memory>
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

    size_t getChildObjectCount() const { return childObjects.size(); }
    bool hasChildObjects() const { return !childObjects.empty(); }
    Object& getChildObjectByIndex(uint32 idx);
    void createChildObject(const char *name);

    size_t getChildOperatorCount() const { return childOperators.size(); }
    bool hasChildOperators() const { return !childOperators.empty(); }
    CsgOperator& getChildOperatorByIndex(uint32 idx);
    void createChildOperator(uint32 id, const char *name, CsgType type);
    void deleteChildOperator(CsgOperator &op);

    //Move CsgOperator into this one
    void moveChildOperator(CsgOperator &parentToMoveFrom, CsgOperator &toMove);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

    CsgType getType() const { return type; }

    bool operator==(const CsgOperator& other) { return id == other.id; }
    bool operator!=(const CsgOperator& other) { return id != other.id; }

protected:
    uint32 id;
    std::string name;
    CsgType type;

    std::vector<std::unique_ptr<Object>> childObjects;
    std::vector<std::unique_ptr<CsgOperator>> childOperators;

    std::vector<std::unique_ptr<CsgOperator>>::iterator findChildOperatorById(const CsgOperator &op);
};