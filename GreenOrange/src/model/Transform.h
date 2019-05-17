#pragma once

#include "SceneEntity.h"

#include <string>

#include "../Types.h"
#include "../DataRepo.h"
#include "../GlslGenerator.h"


class Transform : public SceneEntity
{
public:
    Transform(uint32 id, const char *name, TransformType type);
    TransformType getType() const { return type; }

private:
    TransformType type;
};

class Translation : public Transform {
public:
    Translation(uint32 id, const char *name) :
        Transform(id, name, TransformType::Translation),
        ammount{0.0f} {}

    void setAmmount(float x, float y, float z) { ammount[0] = x; ammount[1] = y; ammount[2] = z; GEN_SET_DIRTY(); }
    void setAmmount(float *dims) { memcpy(ammount, dims, 3 * sizeof(float)); GEN_SET_DIRTY(); }
    float* getAmmount() { return ammount; }

private:
    float ammount[3];
};

class Rotation : public Transform {
public:
    Rotation(uint32 id, const char *name) :
        Transform(id, name, TransformType::Rotation),
        ammount {0.0f} {}

    void setAmmount(float x, float y, float z) { ammount[0] = x; ammount[1] = y; ammount[2] = z; GEN_SET_DIRTY(); }
    void setAmmount(float *dims) { memcpy(ammount, dims, 3 * sizeof(float)); GEN_SET_DIRTY(); }
    float* getAmmount() { return ammount; }

private:
    float ammount[3];
};

class CustomTransform : public Transform {
public:
    CustomTransform(uint32 id, const char *name) :
        Transform(id, name, TransformType::Custom) {}

    void setCode(const char *code) { this->code = code; }
    const std::string& getCode() { return code; }

private:
    std::string code;
};