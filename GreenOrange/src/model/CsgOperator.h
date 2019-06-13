#pragma once

#include "Entity.h"
#include "../Types.h"
#include "../DataRepo.h"


class CsgOperator : public Entity
{
public:
    CsgOperator(uint32 id, const char *name, CsgType type);
    CsgType getType() const { return type; }

private:
    CsgType type;
};