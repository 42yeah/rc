#pragma once

#include "Cartridge.h"
#include <WinSock2.h>
#include <iostream>


class Connect : public Cartridge {
public:
    Connect(std::string server_addr);

    ~Connect();

    virtual void init(App& app) override;

    virtual void sdl_event(App& app, const SDL_Event& e) override;

    virtual void update(App& app) override;

    virtual void destroy(App& app) override;

private:
    std::string server_addr;
    sockaddr_in server_sin;
    bool worker_running;
};

