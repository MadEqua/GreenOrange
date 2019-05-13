#pragma once

#include "Panel.h"

class SceneEntity;

class InspectorPanel : public Panel
{
public:
    InspectorPanel() : Panel(PanelType::Inspector, true) {}

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;
};

