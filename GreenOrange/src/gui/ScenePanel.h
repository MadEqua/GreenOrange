#pragma once

#include "Panel.h"
#include "../Constants.h"
#include "../Types.h"
#include "../model/Scene.h"

class CsgOperator;
class Object;

class ScenePanel : public Panel
{
public:
    ScenePanel() : Panel(PanelType::Scene, true) {}

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;

    void doOperatorNode(Scene &scene, TreeNode<SceneEntity> &node) const;
    void doObjectNode(Scene &scene, TreeNode<SceneEntity> &node) const;

    void doOperatorContextMenu(Scene &scene, TreeNode<SceneEntity> &node) const;
    void doObjectContextMenu(Scene &scene, TreeNode<SceneEntity> &node) const;

    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
    mutable char stringBuffer[STRING_BUFFER_MAX_SIZE] = "";

    struct DndPayload {
        void *dataPtr;
    };
};

