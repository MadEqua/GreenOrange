#pragma once

#include "../DataRepo.h"

class GreenOrange;


class Panel {
public:
    Panel(PanelType type, bool open) : 
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
    PanelType type;
    const char * const name; //Pointer to a literal
    bool open;

    virtual bool internalDrawGui(const GreenOrange &greenOrange) = 0;
};