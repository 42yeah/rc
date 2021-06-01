#include "Client.h"
#include <random>
#include "Packet.h"
#include "Command.h"

unsigned int id_counter = 0;


Client::Client(Clients& clients, sockaddr_in sin) : clients(clients), id(++id_counter), token(""), sin(sin), paired_with(-1) {

}

Client::~Client() {

}

unsigned int Client::get_id() const {
	return id;
}

bool Client::authenticate(std::string in_token) {
	return token == in_token;
}

std::string Client::get_token() {
	std::random_device dev;
	std::uniform_int_distribution distrib(0, 61);
	token = "";
	// Generate a 32bit token, with numbers and letters
	for (int i = 0; i < CLIENT_TOKEN_LEN; i++) {
		int n = distrib(dev);
		if (n < 26) {
			token += 'a' + n;
		}
		else if (n < 52) {
			token += 'A' + (n - 26);
		}
		else {
			token += '0' + (n - 52);
		}
	}
	return token;
}

bool Client::start_frame(unsigned long frame_id, unsigned int frame_len) {
	if (!paired_with.has_value()) {
		return false;
	}
	Client& target_client = paired_with.value().get();
	PacketStream stream;
	stream << ClientCommand::NEW_FRAME << frame_id << frame_len;
	std::string data = stream.get_string();
	sockaddr_in sin = target_client.get_sockaddr_in();
	sendto(clients.get_server_socket(), data.c_str(), data.size(), 0, (sockaddr *) &sin, sizeof(sin));
	return true;
}

bool Client::write_frame(unsigned long frame_id, unsigned int part_id, const char* data, unsigned int part_length) {
	if (!paired_with.has_value()) {
		return false;
	}
	Client& target_client = paired_with.value().get();
	PacketStream stream;
	stream << ClientCommand::FRAME << frame_id << part_id << part_length;
	stream.write(data, part_length);
	std::string send_data = stream.get_string();
	sockaddr_in sin = target_client.get_sockaddr_in();
	sendto(clients.get_server_socket(), send_data.c_str(), send_data.size(), 0, (sockaddr*) &sin, sizeof(sin));
	return true;
}


sockaddr_in Client::get_sockaddr_in() const {
	return sin;
}

bool Client::pair(std::string other_token) {
	auto clients_vec = clients.get_clients();
	auto it = std::find_if(clients_vec.begin(), clients_vec.end(), [other_token](auto& cl) {
		return cl.authenticate(other_token);
	});
	if (it == clients_vec.end()) {
		return false;
	}
	PacketStream pair_stream;
	pair_stream << ClientCommand::PAIR << this->id;
	pair_stream.write(token.c_str(), token.size());
	std::string str = pair_stream.get_string();
	sendto(clients.get_server_socket(), str.c_str(), str.size(), 0, (sockaddr*) &it->sin, sizeof(it->sin));

	it->paired_with = *this;
	return true;
}


Clients::Clients(int& server_socket) : server_socket(server_socket) {
}

Clients::~Clients() {
}

std::optional<std::reference_wrapper<Client>> Clients::register_client(sockaddr_in sin, std::string access_code) {
	if (access_code != ACCESS_CODE) {
		return {};
	}
	Client new_client(*this, sin);
	clients.push_back(new_client);
	return clients[clients.size() - 1];
}

std::optional<std::reference_wrapper<const Client>> Clients::find_client(unsigned int id) const {
	auto it = std::find_if(clients.begin(), clients.end(), [id](auto& cl) {
		return cl.get_id() == id;
	});
	if (it == clients.end()) {
		return {};
	}
	return *it;
}

std::optional<std::reference_wrapper<Client>> Clients::login_client(unsigned int id, std::string token) {
	auto it = std::find_if(clients.begin(), clients.end(), [id, token](auto& cl) {
		return cl.get_id() == id && cl.authenticate(token);
	});
	if (it == clients.end()) {
		return {};
	}
	return *it;
}

const int& Clients::get_server_socket() const {
	return server_socket;
}

const std::vector<Client>& Clients::get_clients() {
	return clients;
}
