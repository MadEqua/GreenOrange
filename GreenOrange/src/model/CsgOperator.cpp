#include "CsgOperator.h"


CsgOperator::CsgOperator(uint32 id, const char *name, CsgType type) :
    Entity(EntityType::CsgOperator, id, name),
    type(type) {
}