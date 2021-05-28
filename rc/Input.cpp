#include "Input.h"
#include <string>
#include "App.h"
#include "Host.h"


void Input::init(App& app) {
	input = "";
	result = InputResult::UNKNOWN;
}

void Input::sdl_event(App& app, const SDL_Event& e) {
	if (e.type != SDL_KEYDOWN) {
		return;
	}
	if (result == InputResult::UNKNOWN) {
		switch (e.key.keysym.sym) {
		case SDL_KeyCode::SDLK_1:
			result = InputResult::CONNECT;
			break;

		case SDL_KeyCode::SDLK_2:
			result = InputResult::HOST;
			break;

		default:
			break;
		}
	} else {
		if (e.key.keysym.sym == SDL_KeyCode::SDLK_BACKSPACE) {
			input = "";
			return;
		} else if (e.key.keysym.sym == SDL_KeyCode::SDLK_RETURN) {
			// Switch cartridge!
			switch (result) {
			case InputResult::UNKNOWN:
				assert("Unknown input result", false);
				break;

			case InputResult::CONNECT:
				assert("Not implemented yet", false);
				break;

			case InputResult::HOST:
				std::unique_ptr<Host> host(new Host(app, input));
				app.switch_cartridge(std::move(host));
				break;
			}
		}
		if (e.key.keysym.sym < 32 || e.key.keysym.sym > 126) {
			return;
		}
		switch (e.key.keysym.mod) {
		case 0:
			input += (char) e.key.keysym.sym;
			break;

		case 1:
			if (e.key.keysym.sym >= 97 && e.key.keysym.sym <= 122) {
				input += (char) e.key.keysym.sym - 'a' + 'A';
			} else if (e.key.keysym.sym == SDL_KeyCode::SDLK_SEMICOLON) {
				input += ':';
			}
			break;
		}
		
	}
}

void Input::update(App& app) {
	const Text& text = app.draw_text("Do you wish to connect or host?");
	const int line_height = text.height;

	if (result == InputResult::UNKNOWN) {
		text.draw(app, 0, 0);
		app.draw_text("1. Connect").draw(app, 0, line_height);
		app.draw_text("2. Host").draw(app, 0, line_height * 2);
	} else {
		app.draw_text("Input server address.").draw(app, 0, 0);
		app.draw_text("Press BACKSPACE to re-enter, and ENTER to submit.").draw(app, 0, line_height);
		app.draw_text(input).draw(app, 0, line_height * 2);
	}
}

void Input::destroy(App& app) {
}
