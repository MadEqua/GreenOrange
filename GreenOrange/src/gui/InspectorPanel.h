#pragma once

#include "Panel.h"
#include "../Constants.h"

class CsgOperator;
class Object;
class Transform;
class Light;

class InspectorPanel : public Panel
{
public:
    InspectorPanel(GuiRoot &guiRoot) : Panel(guiRoot, PanelType::Inspector, true) {}

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;

    void doCsgOperator(CsgOperator &csgOperator);
    void doObject(Object &object);
    void doTransform(Transform &transform);
    void doLight(Light &light);

    mutable char inputBuffer[INPUT_STRING_MAX_SIZE] = "";
};

