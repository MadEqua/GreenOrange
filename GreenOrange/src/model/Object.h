#pragma once

#include "SceneEntity.h"

#include "../Types.h"
#include "DataRepo.h"


class Object : public SceneEntity
{
public:
    Object(uint32 id, const char *name, ObjectType type);

    ObjectType getType() const { return type; }

private:
    ObjectType type;
};