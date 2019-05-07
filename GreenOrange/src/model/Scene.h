#pragma once

#include <string>
#include <vector>
#include <functional>

#include "Object.h"
#include "CsgOperator.h"

class Scene
{
public:
    Scene(const char* name);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

    CsgOperator& getRootCsgOperator() { return unionOperator; }

    void createCsgOperator(const char *name, CsgType type, CsgOperator &parent);
    void deleteCsgOperator(CsgOperator &toDelete);
    void moveCsgOperator(CsgOperator &opToMove, CsgOperator &destination);

    uint32 generateId() { return nextId++; }

private:
    std::string name;

    //A Scene has a Union CsgOperator by default
    CsgOperator unionOperator;

    uint32 nextId;

    bool isCsgOperatorDescendentOf(CsgOperator &op1, CsgOperator &op2);

    //The visitFunction will receive the Node and its parent, in this order
    //If visitFunction returns true the traversal will stop
    void traverseCsgOperatorTree(CsgOperator &root, const std::function<bool(CsgOperator&, CsgOperator*)> &visitFunction);
};

