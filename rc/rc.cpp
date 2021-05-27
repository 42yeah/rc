#include <iostream>
#include <cassert>
#include <SDL.h>
#include "Cartridge.h"
#include "App.h"


int main(int argc, char **argv) {
    App app;
    app.start();

    exit(0); // SDL doesn't seem like it wants to close
    return 0;
}
