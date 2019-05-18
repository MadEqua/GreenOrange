#pragma once

#include "SceneEntity.h"

#include "../Types.h"
#include "../DataRepo.h"
#include "../GlslGenerator.h"
#include "Transform.h"


class Object : public SceneEntity
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
        dimensions {1.0f, 1.0f, 1.0f} {}

    void setDimensions(float x, float y, float z) { dimensions[0] = x; dimensions[1] = y; dimensions[2] = z; GEN_SET_DIRTY(); }
    void setDimensions(float *dims) { memcpy(dimensions, dims, 3 * sizeof(float)); GEN_SET_DIRTY(); }
    float* getDimensions() { return dimensions; }

private:
    float dimensions[3];
};