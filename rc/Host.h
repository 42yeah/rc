#pragma once

#include <iostream>
#include <optional>
#include "common.h"
#include "Cartridge.h"
#include "ScreenCap.h"
#include "ThreadPool.h"


class Host : public Cartridge {
public:
    Host(App &app, std::string server_addr);

    ~Host();

    virtual void init(App& app) override;

    virtual void sdl_event(App& app, const SDL_Event& e) override;

    virtual void update(App& app) override;

    virtual void destroy(App& app) override;

    void send_frame(std::unique_ptr<Buffer> frame);

    bool worker_should_run() const;

    void set_token(unsigned int id, std::string token);

    sockaddr_in get_server_sin() const;

    void set_paired_token(unsigned int id, std::string token);

private:
    std::string server_addr;
    ScreenCap capturer;
    sockaddr_in server_sin;
    bool worker_running;
    std::optional<std::string> token;
    std::optional<int> paired_id;
    std::optional<std::string> paired_token;
    ThreadPool pool;
    unsigned int id;
};

