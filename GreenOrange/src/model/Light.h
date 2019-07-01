#pragma once

#include "Entity.h"

#include "../DataRepo.h"
#include "../GlslGenerator.h"
#include "../Types.h"

#include <glm/glm.hpp>


class Light : public Entity
{
public:
    Light(LightType type, uint32 id, const char *name);

    void setColor(const glm::vec3 &color) { this->color = color; GEN_SET_DIRTY(); }
    const glm::vec3& getColor() const { return color; }

    void setIntensity(float intensity) { this->intensity = intensity; GEN_SET_DIRTY(); }
    float getIntensity() const { return intensity; }
    
    bool isDirectional() const { return type == LightType::Directional; }
    bool isPoint() const { return type == LightType::Point; }

    LightType getType() const { return type; }

    bool isStatic() const { return isStatic_; }
    void setStatic(bool isStatic) { isStatic_ = isStatic; GEN_SET_DIRTY(); }

protected:
    LightType type;
    glm::vec3 color;
    float intensity;
    bool isStatic_ = false;
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