#pragma once

#include <string>
#include <memory>
#include <vector>

#include "../Types.h"
#include "../Assert.h"
#include "../DataRepo.h"
#include "Object.h"


class CsgOperator : public SceneEntity
{
public:
    CsgOperator(uint32 id, const char *name, CsgType type);

    CsgType getType() const { return type; }

    bool isEmpty() const { return childObjects.empty() && childOperators.empty(); }

    size_t getChildObjectCount() const { return childObjects.size(); }
    bool hasChildObjects() const { return !childObjects.empty(); }
    Object& getChildObjectByIndex(uint32 idx);
    void createChildObject(uint32 id, const char *name, ObjectType type);
    void deleteChildObject(Object &object);

    //Move Object into this one
    void moveChildObject(CsgOperator &parentToMoveFrom, Object &toMove);

    size_t getChildOperatorCount() const { return childOperators.size(); }
    bool hasChildOperators() const { return !childOperators.empty(); }
    CsgOperator& getChildOperatorByIndex(uint32 idx);
    void createChildOperator(uint32 id, const char *name, CsgType type);
    void deleteChildOperator(CsgOperator &op);

    //Move CsgOperator into this one
    void moveChildOperator(CsgOperator &parentToMoveFrom, CsgOperator &toMove);

protected:
    CsgType type;

    std::vector<std::unique_ptr<Object>> childObjects;
    std::vector<std::unique_ptr<CsgOperator>> childOperators;

    std::vector<std::unique_ptr<Object>>::iterator findChildObject(const Object &obj);
    std::vector<std::unique_ptr<CsgOperator>>::iterator findChildOperator(const CsgOperator &op);
};