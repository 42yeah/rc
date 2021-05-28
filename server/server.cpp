// server.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <cassert>
#include <WinSock2.h>
#include "App.h"


int main() {
    WSADATA wsa_data;
    assert(WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0);

    App().start();

    return 0;
}
