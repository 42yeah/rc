#include "App.h"
#include "Input.h"
#include "common.h"


App::App() {
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
    assert(TTF_Init() == 0);
    window = nullcheck(SDL_CreateWindow("RC by 42yeah", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL));
    renderer = nullcheck(SDL_CreateRenderer(window, -1, 0));
    font = nullcheck(TTF_OpenFont("ProggyClean.ttf", 18));
    running = true;

    line_height = draw_text("TEST").height;

    // Asks for address to connect initially
    switch_cartridge(std::unique_ptr<Input>(new Input()));
}

App::~App() {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void App::start() {
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e) > 0) {
            switch (e.type) {
            case SDL_QUIT:
                return;
            }
            if (this->cartridge.get() != nullptr) {
                this->cartridge->sdl_event(*this, e);
            }
        }

        SDL_RenderClear(renderer);
        if (this->cartridge.get() != nullptr) {
            this->cartridge->update(*this);
        }
        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60);
    }
}

void App::switch_cartridge(std::unique_ptr<Cartridge> cartridge) {
    if (this->cartridge.get() != nullptr) {
        this->cartridge->destroy(*this);
    }
    this->cartridge.reset(cartridge.release());
    this->cartridge->init(*this);
}

TTF_Font* App::get_font() {
    return font;
}

SDL_Renderer* App::get_renderer() {
    return renderer;
}

const Text& App::draw_text(std::string str) {
    const auto pos = text.find(str);
    if (pos != text.end()) {
        return *pos->second.get();
    }
    text[str] = std::make_unique<Text>(*this, str);
    return *text[str].get();
}

int App::get_line_height() const {
    return line_height;
}

Text::Text(App &app, std::string str) {
    SDL_Surface* surface = TTF_RenderText_Solid(app.get_font(), str.c_str(), SDL_Color{ 255, 255, 255 });
    texture = SDL_CreateTextureFromSurface(app.get_renderer(), surface);

    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
}

Text::~Text() {
    SDL_DestroyTexture(texture);
}

void Text::draw(App& app, int x, int y) const {
    SDL_Rect rect{ x, y, width, height };
    SDL_RenderCopy(app.get_renderer(), texture, nullptr, &rect);
}
