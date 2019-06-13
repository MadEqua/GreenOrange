#pragma once

#include "SceneEntity.h"

#include "../DataRepo.h"
#include "../GlslGenerator.h"
#include "../Types.h"

#include <glm/glm.hpp>


class Light : public SceneEntity
{
public:
    Light(LightType type, uint32 id, const char *name);

    void setColor(const glm::vec3 &color) { this->color = color; GEN_SET_DIRTY(); }
    const glm::vec3& getColor() const { return color; }
    
    bool isDirectional() const { return type == LightType::Directional; }
    bool isPoint() const { return type == LightType::Point; }

    LightType getType() const { return type; }

private:
    LightType type;
    glm::vec3 color;
};


class DirectionalLight : public Light {
public:
    DirectionalLight(uint32 id, const char *name);

    void setDirection(const glm::vec3 &direction) { this->direction = direction; GEN_SET_DIRTY(); }
    const glm::vec3& getDirection() const { return direction; }

private:
    glm::vec3 direction;
};


class PointLight : public Light {
public:
    PointLight(uint32 id, const char *name);

    void setPosition(const glm::vec3 &position) { this->position = position; GEN_SET_DIRTY(); }
    const glm::vec3& getPosition() const { return position; }

private:
    glm::vec3 position;
};