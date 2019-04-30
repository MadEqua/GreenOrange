#include "MainWindow.h"

int main(int argc, char** argv) {
    MainWindow mainWindow;
    if(mainWindow.mainLoop())
        return 0;
    else
        return 1;
}
