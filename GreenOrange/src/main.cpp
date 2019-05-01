#include "GreenOrange.h"
#include "gui/GuiRoot.h"

int main(int argc, char** argv) {

    GreenOrange greenOrange;
    greenOrange.openProject(); //TODO: remove

    GuiRoot guiRoot(greenOrange);

    if(!guiRoot.init())
        return 1;

    guiRoot.run();

    guiRoot.deinit();
    return 0;
}
