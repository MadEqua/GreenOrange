#pragma once

#include "SceneEntity.h"

#include "../DataRepo.h"
#include "../GlslGenerator.h"
#include "../Types.h"


class Light : public SceneEntity
{
public:
    Light(LightType type, uint32 id, const char *name);

    void setColor(float r, float g, float b) { color[0] = r; color[1] = g; color[2] = b; GEN_SET_DIRTY(); }
    void setColor(float *color) { memcpy(color, this->color, 3 * sizeof(float)); GEN_SET_DIRTY(); }
    float* getColor() { return color; }
    bool isDirectional() const { return type == LightType::Directional; }
    bool isPoint() const { return type == LightType::Point; }

    LightType getType() const { return type; }

private:
    LightType type;
    float color[3];
};


class DirectionalLight : public Light {
public:
    DirectionalLight(uint32 id, const char *name);

    void setDirection(float x, float y, float z) { direction[0] = x; direction[1] = y; direction[2] = z; GEN_SET_DIRTY(); }
    void setDirection(float *direction) { memcpy(direction, this->direction, 3 * sizeof(float)); GEN_SET_DIRTY(); }
    float* getDirection() { return direction; }

private:
    float direction[3];
};


class PointLight : public Light {
public:
    PointLight(uint32 id, const char *name);

    void setPosition(float x, float y, float z) { position[0] = x; position[1] = y; position[2] = z; GEN_SET_DIRTY(); }
    void setPosition(float *direction) { memcpy(position, this->position, 3 * sizeof(float)); GEN_SET_DIRTY(); }
    float* getPosition() { return position; }

private:
    float position[3];
};