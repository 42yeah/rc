#pragma once

#include <iostream>
#include "Cartridge.h"
#include "ScreenCap.h"


class Host : public Cartridge {
public:
    Host(App &app, std::string server_addr);

    virtual void init(App& app) override;

    virtual void sdl_event(App& app, const SDL_Event& e) override;

    virtual void update(App& app) override;

    virtual void destroy(App& app) override;

private:
    std::string server_addr;
    ScreenCap capturer;
};

