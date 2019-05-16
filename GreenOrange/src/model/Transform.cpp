#include "Transform.h"


Transform::Transform(uint32 id, const char *name, TransformType type) :
    SceneEntity(SceneEntityType::Transform, id, name),
    type(type) {
}