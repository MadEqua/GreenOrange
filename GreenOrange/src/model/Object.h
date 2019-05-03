#pragma once

#include <string>

class Object
{
public:
    Object(const char *name);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

private:
    std::string name;
};

