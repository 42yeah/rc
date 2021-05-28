#pragma once

#include "Cartridge.h"
#include <iostream>

enum class InputResult {
    UNKNOWN, CONNECT, HOST
};

class Input : public Cartridge {
public:
    Input();

    virtual void init(App& app) override;

    virtual void sdl_event(App& app, const SDL_Event& e) override;

    virtual void update(App& app) override;

    virtual void destroy(App& app) override;

private:
    InputResult result;
    std::string input;
};


