#include "Light.h"


Light::Light(LightType type, uint32 id, const char *name) :
    Entity(EntityType::Light, id, name),
    type(type),
    color(1.0f, 1.0f, 1.0f) {
}


DirectionalLight::DirectionalLight(uint32 id, const char *name) :
    Light(LightType::Directional, id, name),
    direction(0.0f, -1.0f, 0.0f) {
}


PointLight::PointLight(uint32 id, const char *name) :
    Light(LightType::Point, id, name),
    position(0.0f, 0.0f, 0.0f) {
}