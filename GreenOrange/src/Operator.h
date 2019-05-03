#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Object.h"

class Operator
{
public:
    Operator(const char *name, int n);

    int getObjectCount() const { return objects.size(); }
    bool hasObjects() const { return !objects.empty(); }
    Object& getObject(int idx) { return *objects[idx]; }

    int getOperatorCount() const { return operators.size(); }
    bool hasOperators() const { return !operators.empty(); }
    Operator& getOperator(int idx) { return *operators[idx]; }

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

private:
    std::string name;

    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Operator>> operators;
};

