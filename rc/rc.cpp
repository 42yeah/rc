// rc.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <cassert>
#include <SDL.h>
#include "common.h"
#include "Cartridge.h"
#include "App.h"


int main(int argc, char **argv) {
    WSADATA wsa_data;
    assert(0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));

    App app;
    app.start();

    exit(0); // SDL doesn't seem like it wants to close
    return 0;
}
