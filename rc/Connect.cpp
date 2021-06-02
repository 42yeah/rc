#include "Connect.h"
#include <iostream>
#include <string>
#include <thread>
#include "App.h"
#include "Input.h"
#include "Command.h"
#include "Packet.h"

#define TRY_DROP(x, err) if (!x.has_value()) { std::cout << "[ERR] " << err << std::endl; continue; }


Connect::Connect(std::string other_token, std::string server_addr) : other_token(other_token), server_addr(server_addr), worker_running(true), id(0), token({}), pair_state(PairState::PENDING) {
	server_sin.sin_addr.S_un.S_addr = 0;
	server_sin.sin_port = 0;
	server_sin.sin_family = AF_INET;
}

Connect::~Connect() {

}

void receiver(std::reference_wrapper<Connect> ref_connect, std::reference_wrapper<App> ref_app) {
	Connect& connect = ref_connect.get();
	App& app = ref_app.get();
	sockaddr_in sin = connect.get_server_sin();
	int sin_len = sizeof(sin);
	char data[MAXIMUM_PAYLOAD_SIZE] = { 0 };
	while (connect.worker_should_run()) {
		int size = recvfrom(app.get_client_socket(), data, sizeof(data), 0, (sockaddr*)&sin, &sin_len);
		if (size <= 0) {
			std::cerr << "Worker error: wrong recv" << std::endl;
			if (app.get_client_socket() <= 0) {
				std::cerr << "Socket is no longer valid. Quitting." << std::endl;
				break;
			}
			continue;
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
			connect.set_token(id.value(), token_str);

			// Try to pair...
			PacketStream stream;
			stream << Command::CLIENT_PAIR << connect.get_id();
			stream.write(token.value(), CLIENT_TOKEN_LEN);
			stream.write(connect.get_buddy_token().c_str(), CLIENT_TOKEN_LEN);
			std::string data = stream.get_string();
			sendto(app.get_client_socket(), data.c_str(), data.size(), 0, (sockaddr*) &sin, sin_len);
			break;
		}

		case ClientCommand::PAIR_SUCCESS:
			connect.set_pair_state(PairState::PAIRED);
			break;

		case ClientCommand::PAIR_FAILURE:
			connect.set_pair_state(PairState::FAILED);
			break;

		default:
			std::cerr << "Server is sending unsupported command. Dropping." << std::endl;
			break;
		}
	}
}


void Connect::init(App& app) {
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

	std::thread receiver_thread(receiver, std::reference_wrapper<Connect>(*this), std::reference_wrapper<App>(app));
	receiver_thread.detach();

	PacketStream stream;
	unsigned int access_code_len = std::strlen(ACCESS_CODE);
	stream << Command::CLIENTS_REGISTER << access_code_len;
	stream.write(ACCESS_CODE, access_code_len);
	std::string token_request = stream.get_string();
	sendto(app.get_client_socket(), token_request.c_str(), token_request.size(), 0, (sockaddr*)& server_sin, sizeof(server_sin));
}

void Connect::sdl_event(App& app, const SDL_Event& e) {
}

void Connect::update(App& app) {
	if (!token.has_value()) {
		app.draw_text("Trying to obtain token...").draw(app, 0, 0);
		return;
	}
	switch (pair_state) {
	case PairState::PENDING:
		app.draw_text("Token obtained. Trying to pair using buddy code " + other_token).draw(app, 0, 0);
		break;
	
	case PairState::PAIRED:
		app.draw_text("Current pair: " + other_token).draw(app, 0, 0);
		break;

	case PairState::FAILED:
		app.draw_text("Pairing with " + other_token + " failed. Please try again.").draw(app, 0, 0);
		break;
	}
	
}

void Connect::destroy(App& app) {
}

sockaddr_in Connect::get_server_sin() const {
	return server_sin;
}

bool Connect::worker_should_run() const {
	return worker_running;
}

void Connect::set_token(unsigned int id, std::string token) {
	this->id = id;
	this->token = token;
}

std::string Connect::get_buddy_token() const {
	return other_token;
}

unsigned int Connect::get_id() const {
	return id;
}

void Connect::set_pair_state(PairState pair_state) {
	this->pair_state = pair_state;
}

