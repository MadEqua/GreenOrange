#pragma once

#include "../Constants.h"
#include "../Types.h"

class Scene;
class CsgOperator;
class Object;

constexpr int DND_OBJECT = 0;
constexpr int DND_CSG_OPERATOR = 1;

class ScenePanel
{
public:
    void drawGui(Scene &scene) const;

private:
    void doOperatorNode(Scene &scene, CsgOperator &op) const;
    void doObjectNode(Scene &scene, Object &obj) const;

    void doOperatorContextMenu(Scene &scene, CsgOperator &op) const;
    void doObjectContextMenu(Scene &scene, Object &obj) const;

    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
    mutable char stringBuffer[STRING_BUFFER_MAX_SIZE] = "";

    struct DndPayload {
        int type;
        void *dataPtr;
    };
};

