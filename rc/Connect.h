#pragma once

#include "Cartridge.h"
#include "common.h"
#include <iostream>


class Connect : public Cartridge {
public:
    Connect(std::string other_token, std::string server_addr);

    ~Connect();

    virtual void init(App& app) override;

    virtual void sdl_event(App& app, const SDL_Event& e) override;

    virtual void update(App& app) override;

    virtual void destroy(App& app) override;

    sockaddr_in get_server_sin() const;

    bool worker_should_run() const;

    void set_token(unsigned int id, std::string token);

private:
    std::string server_addr;
    sockaddr_in server_sin;
    bool worker_running;
    std::string other_token;
    unsigned int id;
    std::string token;
};

