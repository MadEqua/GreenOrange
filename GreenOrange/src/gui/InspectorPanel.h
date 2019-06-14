#pragma once

#include "Panel.h"
#include "../Constants.h"

class CsgOperator;
class Object;
class Project;
class Transform;
class Light;
class Material;

class InspectorPanel : public Panel
{
public:
    InspectorPanel(GuiRoot &guiRoot) : Panel(guiRoot, PanelType::Inspector, true) {}

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;

    void doCsgOperator(CsgOperator &csgOperator);
    void doObject(Project &project, Object &object);
    void doTransform(Transform &transform);
    void doLight(Light &light);
    void doMaterial(Material &material);

    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
};

