#include "Object.h"


Object::Object(uint32 id, const char *name, ObjectType type) :
    SceneEntity(id, name),
    type(type) {
}