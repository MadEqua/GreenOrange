#include "Scene.h"

#include "CsgOperator.h"
#include "DataRepo.h"


Scene::Scene(int id, const char* name) :
    id(id),
    name(name) {

    op = std::make_unique<CsgOperator>("rootOp", CsgType::Union);
}