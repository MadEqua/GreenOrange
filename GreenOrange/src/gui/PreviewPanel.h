#pragma once

#include "../gl/PreviewRenderer.h"

class PreviewPanel
{
public:
    PreviewPanel();

    void drawGui();

private:
    PreviewRenderer previewRenderer;
};

