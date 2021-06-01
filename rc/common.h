#pragma once

#include <cassert>
#include <WS2tcpip.h>


template<typename T>
T nullcheck(T input) {
    assert(input);
    return input;
}
