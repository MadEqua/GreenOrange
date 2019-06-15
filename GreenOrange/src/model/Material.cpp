#include "Material.h"


Material::Material(uint32 id, const char *name) :
    Entity(EntityType::Material, id, name),
    baseColor(0.5f, 0.0f, 0.5f),
    metallic(0.0f),
    roughness(0.0f),
    emissiveColor(0.0f),
    emissiveIntensity(0.0f) {
}