#pragma once

#include <memory>
#include <vector>

#include "../Types.h"
#include "../DataRepo.h"
#include "SceneEntity.h"


class Transform
{
public:
    Transform();

    TransformType getType() const { return type; }

private:
    TransformType type;
    std::vector<std::unique_ptr<Transform>> childTransforms;
    std::vector<std::unique_ptr<SceneEntity>> childSceneEntities;
};

