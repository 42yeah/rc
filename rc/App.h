#pragma once

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <cassert>
#include "common.h"
#include "Cartridge.h"


class Text {
public:
    Text(App &app, std::string str);

    ~Text();

    void draw(App &app, int x, int y) const;

    SDL_Texture* texture;
    int width, height;
};

class App {
public:
    App();

    ~App();

    void start();

    void switch_cartridge(std::unique_ptr<Cartridge> cartridge);

    TTF_Font* get_font();

    SDL_Renderer* get_renderer();

    const Text& draw_text(std::string str);

    int get_line_height() const;

    int get_client_socket() const;

private:
    std::map<std::string, std::unique_ptr<Text>> text;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<Cartridge> cartridge;
    TTF_Font* font;
    bool running;
    int line_height;
    int client_socket;
};

