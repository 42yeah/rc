#pragma once

#include <iostream>
#include <vector>
#include <optional>

#define CLIENT_TOKEN_LEN 32
#define ACCESS_CODE "access_code"


class Client {
public:
	Client();

	~Client();

	unsigned int get_id() const;

	bool authenticate(std::string in_token);

	std::string get_token();

private:
	unsigned int id;
	std::string token;
};

class Clients {
public:
	Clients();

	~Clients();
	
	std::optional<std::reference_wrapper<Client>> register_client(std::string access_code);

	std::optional<std::reference_wrapper<const Client>> find_client(unsigned int id);

	std::optional<std::reference_wrapper<Client>> login_client(unsigned int id, std::string token);

private:
	std::vector<Client> clients;
};