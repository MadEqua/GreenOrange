#pragma once

#include <string>

#include "../Types.h"


class SceneEntity
{
public:
    SceneEntity(uint32 id, const char *name);
    virtual ~SceneEntity() = default;

    uint32 getId() const { return id; }

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; }

    bool operator==(const SceneEntity& other) { return id == other.id; }
    bool operator!=(const SceneEntity& other) { return id != other.id; }

protected:
    uint32 id;
    std::string name;
};

