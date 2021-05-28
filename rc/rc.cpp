// rc.cpp : Ce fichier contient la fonction 'main'. L'ex�cution du programme commence et se termine � cet endroit.
//

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
