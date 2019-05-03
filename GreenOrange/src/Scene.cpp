#include "Scene.h"

#include "Operator.h"


Scene::Scene(int id, const char* name) :
    id(id),
    name(name) {

    op = std::make_unique<Operator>("rootOp", 3);
}