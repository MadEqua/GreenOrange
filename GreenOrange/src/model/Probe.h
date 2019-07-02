#pragma once

#include "Entity.h"
#include "../Types.h"
#include "../GlslGenerator.h"

#include <glm/glm.hpp>


class Probe : public Entity
{
public:
    Probe(uint32 id, const char *name);

    void setPosition(const glm::vec3 &pos) { position = pos; PREVIEW_SET_DIRTY(); }
    const glm::vec3& getPosition() const { return position; }

private:
    glm::vec3 position;
};