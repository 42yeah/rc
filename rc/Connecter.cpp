#include "Connecter.h"
#include <iostream>
#include <string>
#include "App.h"
#include "Input.h"
#include "Command.h"
#include "Packet.h"


Connecter::Connecter(std::string other_token, std::string server_addr) : other_token(other_token), server_addr(server_addr), worker_running(true), id(0), token("") {
	server_sin.sin_addr.S_un.S_addr = 0;
	server_sin.sin_port = 0;
	server_sin.sin_family = AF_INET;
}

Connecter::~Connecter() {

}

void receiver(std::reference_wrapper<Connecter> ref_connect, std::reference_wrapper<App> ref_app) {
	Connecter& connecter = ref_connect.get();
	App& app = ref_app.get();
	sockaddr_in sin = connecter.get_server_sin();
	int sin_len = sizeof(sin);
	char data[MAXIMUM_PAYLOAD_SIZE] = { 0 };
	while (connecter.worker_should_run()) {
		int size = recvfrom(app.get_client_socket(), data, sizeof(data), 0, (sockaddr*)&sin, &sin_len);
		if (size <= 0) {
			std::cerr << "Worker error: wrong recv" << std::endl;
			break;
		}
		Packet packet(data, size);
		auto command = packet.next<ClientCommand>();
		// TRY_DROP(command, "the server gave no command");
		switch (command.value()) {
		case ClientCommand::NOP:
			break;

		case ClientCommand::SET_TOKEN: {
			auto id = packet.next<unsigned int>();
			// TRY_DROP(id, "id is empty");
			auto token = packet.next_ptr<char>(CLIENT_TOKEN_LEN);
			// TRY_DROP(token, "token is empty or invalid");
			std::string token_str(token.value(), CLIENT_TOKEN_LEN);
			connecter.set_token(id.value(), token_str);
			break;
		}

		default:
			std::cerr << "Server is sending unsupported command. Dropping." << std::endl;
			break;
		}
	}
}


void Connecter::init(App& app) {
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
	if (other_token.size() != CLIENT_TOKEN_LEN || port_column >= server_addr.size() || ip.empty() || port.empty() || ip_it != ip.end() || port_it != port.end()) {
		app.switch_cartridge(std::make_unique<Input>());
		return;
	}
	inet_pton(AF_INET, ip.c_str(), (in_addr *) &server_sin.sin_addr);
	server_sin.sin_port = htons(std::stoi(port));
}

void Connecter::sdl_event(App& app, const SDL_Event& e) {
}

void Connecter::update(App& app) {
}

void Connecter::destroy(App& app) {
}

sockaddr_in Connecter::get_server_sin() const {
	return server_sin;
}

bool Connecter::worker_should_run() const {
	return worker_running;
}

void Connecter::set_token(unsigned int id, std::string token) {
	this->id = id;
	this->token = token;
}

