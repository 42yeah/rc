// rc.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
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
