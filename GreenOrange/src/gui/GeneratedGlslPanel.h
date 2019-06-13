#pragma once

#include "Panel.h"

class GreenOrange;


class GeneratedGlslPanel : public Panel
{
public:
    GeneratedGlslPanel(GuiRoot &guiRoot) : Panel(guiRoot, PanelType::GeneratedGlsl, true) {}

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;
};