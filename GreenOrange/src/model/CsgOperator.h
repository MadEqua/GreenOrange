#pragma once

#include <memory>
#include <vector>

#include "../Types.h"
#include "Object.h"


class CsgOperator : public SceneEntity
{
public:
    CsgOperator(uint32 id, const char *name, CsgType type);

    CsgType getType() const { return type; }

    size_t getChildObjectCount() const { return childObjects.size(); }
    bool hasChildObjects() const { return !childObjects.empty(); }
    Object& getChildObjectByIndex(uint32 idx);
    void createChildObject(uint32 id, const char *name, ObjectType type);
    void deleteChildObject(Object &object);
    void clearChildObjects();

    //Move Object into this one
    void moveChildObject(CsgOperator &parentToMoveFrom, Object &toMove);

    size_t getChildOperatorCount() const { return childOperators.size(); }
    bool hasChildOperators() const { return !childOperators.empty(); }
    CsgOperator& getChildOperatorByIndex(uint32 idx);
    void createChildOperator(uint32 id, const char *name, CsgType type);
    void deleteChildOperator(CsgOperator &op);
    void clearChildOperators();

    //Move CsgOperator into this one
    void moveChildOperator(CsgOperator &parentToMoveFrom, CsgOperator &toMove);

    size_t getChildCount() const { return childOperators.size() + childObjects.size(); }
    bool hasChildren() const { return !childOperators.empty() || !childObjects.empty(); }
    void deleteChildren();

protected:
    CsgType type;

    std::vector<std::unique_ptr<Object>> childObjects;
    std::vector<std::unique_ptr<CsgOperator>> childOperators;

    std::vector<std::unique_ptr<Object>>::iterator findChildObject(const Object &obj);
    std::vector<std::unique_ptr<CsgOperator>>::iterator findChildOperator(const CsgOperator &op);
};