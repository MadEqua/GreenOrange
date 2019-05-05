#include "Scene.h"

#include "CsgOperator.h"


Scene::Scene(int id, const char* name) :
    id(id),
    name(name) {

    op = std::make_unique<CsgOperator>("rootOp", CSG_OPERATOR_LIST[0]);
}