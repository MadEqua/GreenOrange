#pragma once

#include "Entity.h"

#include "../Types.h"
#include "../DataRepo.h"
#include "../GlslGenerator.h"
#include "Transform.h"
#include "Material.h"

#include <glm/glm.hpp>


class Object : public Entity
{
public:
    Object(uint32 id, const char *name, ObjectType type);
    ObjectType getType() const { return type; }

    void attachToTransform(Transform &t) { transformId = t.getId(); }
    uint32 getTransformId() { return transformId; }
    void detachFromTransform() { transformId = -1; }
    bool isAttachedToTransform() const { return transformId != -1; }

    void attachToMaterial(Material &m) { materialId = m.getId(); GEN_SET_DIRTY(); }
    uint32 getMaterialId() { return materialId; }
    void detachFromMaterial() { materialId = -1; }
    bool isAttachedToMaterial() const { return materialId != -1; }

private:
    ObjectType type;
    uint32 transformId = -1;
    uint32 materialId = -1;
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