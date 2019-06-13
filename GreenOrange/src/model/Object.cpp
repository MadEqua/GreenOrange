#include "Object.h"


Object::Object(uint32 id, const char *name, ObjectType type) :
    Entity(EntityType::Object, id, name),
    type(type) {
}