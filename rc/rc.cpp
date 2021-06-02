// rc.cpp : Ce fichier contient la fonction 'main'. L'ex�cution du programme commence et se termine � cet endroit.
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

    return 0;
}
