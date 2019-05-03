#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Object.h"
#include "Operator.h"

class Scene
{
public:
    explicit Scene(int id, const char* name);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

    int getId() const { return id; }

    bool isEmpty() const { return op && object; }
    bool hasOperator() const { return static_cast<bool>(op); }
    bool hasObject() const { return static_cast<bool>(object); }

    Object* getObject() const { return object.get(); }
    Operator* getOperator() const { return op.get(); }

private:
    int id; //Unique
    std::string name;
    
    //A Scene has either one Operator or one Object
    std::unique_ptr<Operator> op;
    std::unique_ptr<Object> object;
};

