#pragma once

#include <iostream>
#include <optional>
#include "Cartridge.h"
#include "ScreenCap.h"


class Host : public Cartridge {
public:
    Host(App &app, std::string server_addr);

    virtual void init(App& app) override;

    virtual void sdl_event(App& app, const SDL_Event& e) override;

    virtual void update(App& app) override;

    virtual void destroy(App& app) override;

    bool worker_should_run() const;

    void set_token(unsigned int id, std::string token);

    sockaddr_in get_server_sin() const;

private:
    std::string server_addr;
    ScreenCap capturer;
    sockaddr_in server_sin;
    bool worker_running;
    std::optional<std::string> token;
    unsigned int id;
};

