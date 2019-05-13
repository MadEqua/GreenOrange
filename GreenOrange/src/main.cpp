#include "gl/GLinit.h"
#include "model/GreenOrange.h"
#include "gui/GuiRoot.h"
#include "DataRepo.h"


int main(int argc, char** argv) {

    if(!initGlfw() || !initGlad())
        return 1;

    initDataRepo();

    GreenOrange greenOrange;
    GuiRoot guiRoot(greenOrange, *glfwWindow);

    if(!guiRoot.init())
        return 1;

    glfwMakeContextCurrent(glfwWindow);
    while(!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();
        guiRoot.drawFrame();
        glfwSwapBuffers(glfwWindow);
    }

    guiRoot.deinit();
    deinitGlfw();
    return 0;
}