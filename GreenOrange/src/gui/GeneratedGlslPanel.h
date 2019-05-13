#pragma once

#include "Panel.h"

class GreenOrange;


class GeneratedGlslPanel : public Panel
{
public:
    GeneratedGlslPanel() : Panel(PanelType::GeneratedGlsl, false) {}

private:
    bool internalDrawGui(const GreenOrange &greenOrange) override;
};