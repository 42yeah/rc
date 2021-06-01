#include "Host.h"
#include "App.h"
#include "Input.h"
#include <iostream>
#include <string>
#include <thread>
#include "Packet.h"
#include "Command.h"

#define TRY_DROP(x, err) if (!x.has_value()) { std::cout << "[ERR] " << err << std::endl; continue; }


void receiver(std::reference_wrapper<Host> ref_host, std::reference_wrapper<App> ref_app) {
	Host& host = ref_host.get();
	App& app = ref_app.get();
	sockaddr_in sin = host.get_server_sin();
	int sin_len = sizeof(sin);
	char data[MAXIMUM_PAYLOAD_SIZE] = { 0 };
	while (host.worker_should_run()) {
		int size = recvfrom(app.get_client_socket(), data, sizeof(data), 0, (sockaddr *) &sin, &sin_len);
		if (size <= 0) {
			std::cerr << "Worker error: wrong recv" << std::endl;
			break;
		}
		Packet packet(data, size);
		auto command = packet.next<ClientCommand>();
		TRY_DROP(command, "the server gave no command");
		switch (command.value()) {
		case ClientCommand::NOP:
			break;

		case ClientCommand::SET_TOKEN: {
			auto id = packet.next<unsigned int>();
			TRY_DROP(id, "id is empty");
			auto token = packet.next_ptr<char>(CLIENT_TOKEN_LEN);
			TRY_DROP(token, "token is empty or invalid");
			std::string token_str(token.value(), CLIENT_TOKEN_LEN);
			host.set_token(id.value(), token_str);
			break;
		}

		case ClientCommand::PAIR: {
			auto id = packet.next<unsigned int>();
			TRY_DROP(id, "paired_id is empty");
			auto token = packet.next_ptr<char>(CLIENT_TOKEN_LEN);
			TRY_DROP(token, "paired_token is empty");
			host.set_paired_token(id.value(), token.value());
			break;
		}

		default:
			std::cerr << "Server is sending unsupported command. Dropping." << std::endl;
			break;
		}
	}
}

Host::Host(App& app, std::string server_addr) : server_addr(server_addr), capturer(app.get_renderer()), worker_running(true), id(0), token({}), paired_id({}), paired_token({}) {
	server_sin.sin_family = AF_INET;
	server_sin.sin_port = 0;
	server_sin.sin_addr.S_un.S_addr = 0;
}

void Host::init(App& app) {
	auto port_column = server_addr.find(':', 0);
	std::string ip = server_addr.substr(0, port_column);
	std::string port = server_addr.substr(port_column + 1, server_addr.size());

	auto ip_it = ip.begin();
	while (ip_it != ip.end() && (std::isdigit(*ip_it) || *ip_it == '.')) {
		ip_it++;
	}
	auto port_it = port.begin();
	while (port_it != port.end() && std::isdigit(*port_it)) {
		port_it++;
	}
	if (port_column >= server_addr.size() || ip.empty() || port.empty() || ip_it != ip.end() || port_it != port.end()) {
		app.switch_cartridge(std::make_unique<Input>());
		return;
	}
	inet_pton(AF_INET, ip.c_str(), (in_addr*) &server_sin.sin_addr);
	server_sin.sin_port = htons(std::stoi(port));

	std::thread receiver(::receiver, std::reference_wrapper<Host>(*this), std::reference_wrapper<App>(app));
	receiver.detach();

	// Send a request for token to server
	PacketStream token_request;
	unsigned int access_code_len = std::strlen(ACCESS_CODE);
	token_request << Command::CLIENTS_REGISTER << access_code_len;
	token_request.write(ACCESS_CODE, access_code_len);
	std::string token_data = token_request.get_string();
	sendto(app.get_client_socket(), token_data.c_str(), token_data.size(), 0, (sockaddr*) &server_sin, sizeof(server_sin));

	capturer.get_jpeg_data();
}

void Host::sdl_event(App& app, const SDL_Event& e) {
}

void Host::update(App& app) {
	capturer.shot(app.get_renderer());
	std::unique_ptr<Buffer> data = capturer.get_jpeg_data();

	SDL_RenderCopyEx(app.get_renderer(), capturer.get_texture(), nullptr, nullptr, 0.0, nullptr, SDL_FLIP_VERTICAL);
	if (!token.has_value()) {
		app.draw_text("Now serving as HOST. Access code: PENDING.").draw(app, 0, 0);
	} else if (!paired_token.has_value()) {
		app.draw_text("Now serving as HOST. Access code: " + token.value() + ". Waiting for buddy.").draw(app, 0, 0);
	} else {
		app.draw_text("Current pair: " + paired_token.value()).draw(app, 0, 0);
	}
	
	app.draw_text(std::string("Frame size: ") + std::to_string(data->len)).draw(app, 0, app.get_line_height());
}

void Host::destroy(App& app) {

}

bool Host::worker_should_run() const {
	return worker_running;
}

void Host::set_token(unsigned int id, std::string token) {
	this->id = id;
	this->token = token;
}

sockaddr_in Host::get_server_sin() const {
	return server_sin;
}

void Host::set_paired_token(unsigned int id, std::string token) {
	this->paired_id = id;
	this->paired_token = token;
}
