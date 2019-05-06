#pragma once

#include "../Constants.h"

class Scene;
class CsgOperator;

class ScenePanel
{
public:
    void drawGui(Scene &scene);

private:
    void doOperatorNode(Scene &scene, CsgOperator &op) const;
    void doOperatorContextMenu(Scene &scene, CsgOperator &op) const;

    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
    mutable char stringBuffer[STRING_BUFFER_MAX_SIZE] = "";
};

