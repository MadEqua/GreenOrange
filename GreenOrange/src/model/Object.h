#pragma once

#include "Entity.h"

#include "../Types.h"
#include "../DataRepo.h"
#include "../GlslGenerator.h"
#include "Transform.h"

#include <glm/glm.hpp>


class Object : public Entity
{
public:
    Object(uint32 id, const char *name, ObjectType type);
    ObjectType getType() const { return type; }

    void attachToTransform(Transform &t) { parentTransformId = t.getId(); }
    uint32 getTransformId() { return parentTransformId; }
    void detachFromTransform() { parentTransformId = -1; }
    bool isAttachedToTransform() const { return parentTransformId != -1; }

private:
    ObjectType type;
    uint32 parentTransformId = -1;
};


class Sphere : public Object {
public:
    Sphere(uint32 id, const char *name) :
        Object(id, name, ObjectType::Sphere),
        radius(1.0f) {}

    void setRadius(float r) { radius = r; GEN_SET_DIRTY(); }
    float getRadius() const { return radius; }

private:
    float radius;
};

class Box : public Object {
public:
    Box(uint32 id, const char *name) :
        Object(id, name, ObjectType::Box),
        dimensions(1.0f, 1.0f, 1.0f) {}

    void setDimensions(const glm::vec3 &dimensions) { this->dimensions = dimensions; GEN_SET_DIRTY(); }
    const glm::vec3& getDimensions() const { return dimensions; }

private:
    glm::vec3 dimensions;
};