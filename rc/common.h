#pragma once

#include <cassert>


template<typename T>
T nullcheck(T input) {
    assert(input);
    return input;
}
