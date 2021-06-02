#pragma once

#include <SDL.h>

class App;

// Cartrigde is an FSM.
class Cartridge {
public:
    virtual ~Cartridge() {};

    virtual void init(App& app) = 0;

    virtual void sdl_event(App& app, const SDL_Event& e) = 0;

    virtual void update(App& app) = 0;

    virtual void destroy(App& app) = 0;
};

