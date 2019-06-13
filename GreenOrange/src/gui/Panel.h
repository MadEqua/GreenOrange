#pragma once

#include "../DataRepo.h"

class GreenOrange;
class GuiRoot;


class Panel {
public:
    Panel(GuiRoot &guiRoot, PanelType type, bool open) :
        guiRoot(guiRoot),
        type(type),
        name(PanelTypeStrings[static_cast<int>(type)]),
        open(open) {}

    virtual ~Panel() = default;

    void drawGui(const GreenOrange &greenOrange) {
        if(open)
            open = internalDrawGui(greenOrange);
    }

    const char* getName() const { return name; }
    
    bool isOpen() const { return open; }
    void flipOpen() { open = !open; }

protected:
    GuiRoot &guiRoot;

    PanelType type;
    const char * const name; //Pointer to a literal
    bool open;

    virtual bool internalDrawGui(const GreenOrange &greenOrange) = 0;
};