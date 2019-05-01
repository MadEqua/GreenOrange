#pragma once

#include <string>

class Scene
{
public:
    explicit Scene(int id, const char* name);

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

    int getId() const { return id; }

private:
    int id; //Unique
    std::string name;
};

