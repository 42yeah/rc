#include "Host.h"
#include "App.h"


Host::Host(App &app, std::string server_addr) : server_addr(server_addr), capturer(app.get_renderer()) {
}

void Host::init(App& app) {
	
}

void Host::sdl_event(App& app, const SDL_Event& e) {
}

void Host::update(App& app) {
	capturer.shot(app.get_renderer());
	SDL_RenderCopyEx(app.get_renderer(), capturer.get_texture(), nullptr, nullptr, 0.0, nullptr, SDL_FLIP_VERTICAL);
	app.draw_text("Now serving as HOST. Access code: PENDING.").draw(app, 0, 0);
}

void Host::destroy(App& app) {
}
