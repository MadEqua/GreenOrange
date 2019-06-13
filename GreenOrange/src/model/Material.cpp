#include "Material.h"


Material::Material(uint32 id, const char *name) :
    Entity(EntityType::Material, id, name),
    color(0.5f, 0.0f, 0.5f) {
}