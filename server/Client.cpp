#include "Client.h"
#include <random>

unsigned int id_counter = 0;


Client::Client() : id(++id_counter), token("") {

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

Clients::Clients() {
}

Clients::~Clients() {
}

std::optional<std::reference_wrapper<Client>> Clients::register_client(std::string access_code) {
	if (access_code != ACCESS_CODE) {
		return {};
	}
	Client new_client;
	clients.push_back(new_client);
	return clients[clients.size() - 1];
}

std::optional<std::reference_wrapper<const Client>> Clients::find_client(unsigned int id) {
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
