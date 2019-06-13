#pragma once

#include <string>

#include "../Types.h"
#include "../GlslGenerator.h"
#include "../DataRepo.h"


class Entity
{
public:
    virtual ~Entity() = default;

    uint32 getId() const { return id; }

    const std::string& getName() const { return name; }
    void setName(const char* newName) { name = newName; GEN_SET_DIRTY(); }

    EntityType getType() const { return type; }
    bool isCsgOperator() const { return type == EntityType::CsgOperator; }
    bool isObject() const { return type == EntityType::Object; }
    bool isTransform() const { return type == EntityType::Transform; }
    bool isLight() const { return type == EntityType::Light; }
    bool isMaterial() const { return type == EntityType::Material; }

    bool operator==(const Entity& other) { return id == other.id; }
    bool operator!=(const Entity& other) { return id != other.id; }

protected:
    Entity(EntityType type, uint32 id, const char *name);

    uint32 id;
    std::string name;
    EntityType type;
};

