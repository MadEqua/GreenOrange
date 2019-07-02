#pragma once

#include <glm/glm.hpp>

#include "Entity.h"
#include "../GlslGenerator.h"


class Material : public Entity
{
public:
    Material(uint32 id, const char *name);

    void setBaseColor(const glm::vec3 &baseColor) { this->baseColor = baseColor; PREVIEW_SET_DIRTY(); }
    const glm::vec3& getBaseColor() const { return baseColor; }

    void setMetallic(float metallic) { this->metallic = metallic; PREVIEW_SET_DIRTY(); }
    float getMetallic() const { return metallic; }

    void setRoughness(float roughness) { this->roughness = roughness; PREVIEW_SET_DIRTY(); }
    float getRoughness() const { return roughness; }

    void setEmissiveColor(const glm::vec3 &emissiveColor) { this->emissiveColor = emissiveColor; PREVIEW_SET_DIRTY(); }
    const glm::vec3& getEmissiveColor() const { return emissiveColor; }

    void setEmissiveIntensity(float emissiveIntensity) { this->emissiveIntensity = emissiveIntensity; PREVIEW_SET_DIRTY(); }
    float getEmissiveIntensity() const { return emissiveIntensity; }

private:
    glm::vec3 baseColor;
    float metallic;
    float roughness;
    //glm::vec3 reflectance;
    glm::vec3 emissiveColor;
    float emissiveIntensity;
};

