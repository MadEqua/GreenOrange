#pragma once

#include "../Constants.h"

class Scene;
class CsgOperator;

class ScenePanel
{
public:
    void drawGui(Scene &scene);

private:
    void doOperatorNode(CsgOperator &op) const;
    void doOperatorContextMenu(CsgOperator &op) const;

    mutable char buffer[STRING_MAX_SIZE] = "";
};

