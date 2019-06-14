#pragma once

#include <glm/glm.hpp>

#include "Entity.h"
#include "../GlslGenerator.h"


class Material : public Entity
{
public:
    Material(uint32 id, const char *name);

    void setColor(const glm::vec3 &color) { this->color = color; GEN_SET_DIRTY(); }
    const glm::vec3& getColor() const { return color; }

private:
    glm::vec3 color;
};

