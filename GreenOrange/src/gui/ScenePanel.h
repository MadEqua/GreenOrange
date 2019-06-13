#pragma once

#include <imgui.h>

#include "Panel.h"
#include "../Constants.h"
#include "../Types.h"
#include "../model/Scene.h"

class CsgOperator;
class Object;


class ScenePanel : public Panel
{
public:
    ScenePanel(GuiRoot &guiRoot) : Panel(guiRoot, PanelType::Scene, true) {}

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;

    void doOperatorNode(Scene &scene, TreeNode<Entity> &node) const;
    void doOperatorContextMenu(Scene &scene, TreeNode<Entity> &node) const;
    
    void doObjectNode(Scene &scene, TreeNode<Entity> &node) const;
    void doObjectContextMenu(Scene &scene, TreeNode<Entity> &node) const;
    
    void doLightContextMenu(Scene &scene, Light &light) const;

    void doTransformNode(Scene &scene, uint32 treeIndex, TreeNode<Transform> &node) const;
    void doTransformContextMenu(Scene &scene, uint32 treeIndex, TreeNode<Transform> &node) const;
    void doTransformAttachments(Scene &scene, uint32 treeIndex, TreeNode<Transform> &node) const;

    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
    mutable char stringBuffer[STRING_BUFFER_MAX_SIZE] = "";

    const ImVec4 COLOR_BLUE {0.6f, 0.6f, 0.95f, 1.0f};
    const ImVec4 COLOR_GRAY {0.7f, 0.7f, 0.7f, 1.0f};

    struct DndPayload {
        enum class DndType {
            Csg,
            Object,
            Transform,
            TransformAtttachment
        };
        DndType type;
        int intData;
        void *dataPtr;
    };
};

