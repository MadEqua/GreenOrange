#include "Probe.h"


Probe::Probe(uint32 id, const char *name) :
    Entity(EntityType::Probe, id, name),
    position(0.0f) {
}