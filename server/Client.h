#pragma once

#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <optional>

#define CLIENT_TOKEN_LEN 8
#define ACCESS_CODE "access_code"


class Clients;

class Client {
public:
	Client(Clients& clients, sockaddr_in sin);

	~Client();

	unsigned int get_id() const;

	bool authenticate(std::string in_token);

	std::string get_token();

	bool write_frame(unsigned long frame_id, unsigned int frame_len, unsigned int part_id, const char* data, unsigned int part_length);

	sockaddr_in get_sockaddr_in() const;

	bool pair(std::string other_token);

private:
	unsigned int id;
	std::optional<std::reference_wrapper<Client>> paired_with;
	std::string token;
	sockaddr_in sin;
	Clients& clients;
};

class Clients {
public:
	Clients(int& server_socket);

	~Clients();
	
	std::optional<std::reference_wrapper<Client>> register_client(sockaddr_in sin, std::string access_code);

	std::optional<std::reference_wrapper<const Client>> find_client(unsigned int id) const;

	std::optional<std::reference_wrapper<Client>> login_client(unsigned int id, std::string token);

	const int& get_server_socket() const;

	const std::vector<Client>& get_clients();

private:
	int& server_socket;
	std::vector<Client> clients;
};