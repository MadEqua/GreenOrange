#include "Entity.h"


Entity::Entity(EntityType type, uint32 id, const char *name) :
    type(type),
    id(id),
    name(name) {
}