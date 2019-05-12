#include "SceneEntity.h"


SceneEntity::SceneEntity(SceneEntityType type, uint32 id, const char *name) :
    type(type),
    id(id),
    name(name) {
}