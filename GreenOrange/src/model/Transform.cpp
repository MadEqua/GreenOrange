#include "Transform.h"


Transform::Transform(uint32 id, const char *name, TransformType type) :
    Entity(EntityType::Transform, id, name),
    type(type) {
}