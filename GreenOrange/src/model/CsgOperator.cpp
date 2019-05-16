#include "CsgOperator.h"


CsgOperator::CsgOperator(uint32 id, const char *name, CsgType type) :
    SceneEntity(SceneEntityType::CsgOperator, id, name),
    type(type) {
}