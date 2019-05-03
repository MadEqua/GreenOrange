#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../Types.h"
#include "../Assert.h"
#include "Object.h"

class Operator
{
public:
    Operator(const char *name, int n);

    size_t getObjectCount() const { return objects.size(); }
    bool hasObjects() const { return !objects.empty(); }
    Object& getObject(uint32 idx) { GO_ASSERT(idx < objects.size()); return *objects[idx]; }

    size_t getOperatorCount() const { return operators.size(); }
    bool hasOperators() const { return !operators.empty(); }
    Operator& getOperator(uint32 idx) { GO_ASSERT(idx < operators.size()); return *operators[idx]; }

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

private:
    std::string name;

    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Operator>> operators;
};

