#include "Host.h"
#include "App.h"


Host::Host(std::string server_addr) : server_addr(server_addr) {
}

void Host::init(App& app) {
}

void Host::sdl_event(App& app, const SDL_Event& e) {
}

void Host::update(App& app) {
	app.draw_text("Now serving as HOST. Access code: PENDING.").draw(app, 0, 0);
}

void Host::destroy(App& app) {
}
