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

    void setIrradianceCoeff(int band, float r, float  g, float b) { GO_ASSERT(band < 9); irradianceSHCoeffs[band] = glm::vec3(r, g, b); PREVIEW_SET_DIRTY(); }
    const glm::vec3* getIrradianceCoeffs() const { return irradianceSHCoeffs; }

private:
    glm::vec3 position;
    glm::vec3 irradianceSHCoeffs[9];
};