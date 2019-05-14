#pragma once

#include <string>

#include "../Types.h"
#include "../GlslGenerator.h"
#include "../DataRepo.h"


class SceneEntity
{
public:
    SceneEntity(SceneEntityType type, uint32 id, const char *name);
    virtual ~SceneEntity() = default;

    uint32 getId() const { return id; }

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; GEN_SET_DIRTY() }

    SceneEntityType getType() const { return type; }
    bool isCsgOperator() const { return type == SceneEntityType::CsgOperator; }
    bool isObject() const { return type == SceneEntityType::Object; }

    bool operator==(const SceneEntity& other) { return id == other.id; }
    bool operator!=(const SceneEntity& other) { return id != other.id; }

protected:
    uint32 id;
    std::string name;
    SceneEntityType type;
};

