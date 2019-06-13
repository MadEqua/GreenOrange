#pragma once

#include "SceneEntity.h"

#include <string>

#include "../Types.h"
#include "../DataRepo.h"
#include "../GlslGenerator.h"

#include <glm/glm.hpp>


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

    void setAmmount(const glm::vec3 &ammount) { this->ammount = ammount; GEN_SET_DIRTY(); }
    const glm::vec3& getAmmount() const { return ammount; }

private:
    glm::vec3 ammount;
};

class Rotation : public Transform {
public:
    Rotation(uint32 id, const char *name) :
        Transform(id, name, TransformType::Rotation),
        ammount {0.0f} {}

    void setAmmount(const glm::vec3 &ammount) { this->ammount = ammount; GEN_SET_DIRTY(); }
    const glm::vec3& getAmmount() const { return ammount; }

private:
    glm::vec3 ammount;
};

class CustomTransform : public Transform {
public:
    CustomTransform(uint32 id, const char *name) :
        Transform(id, name, TransformType::Custom),
        code("p") {}

    void setCode(const char *code) { this->code = code; GEN_SET_DIRTY(); }
    const std::string& getCode() { return code; }

private:
    std::string code;
};