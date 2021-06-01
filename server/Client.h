#pragma once

#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <optional>

#define CLIENT_TOKEN_LEN 32
#define ACCESS_CODE "access_code"

class Clients;

class Client {
public:
	Client(Clients& clients, sockaddr_in sin);

	~Client();

	unsigned int get_id() const;

	bool authenticate(std::string in_token);

	std::string get_token();

	bool start_frame(unsigned int target_id, unsigned long frame_id, unsigned int frame_len);

	bool write_frame(unsigned int target_id, unsigned long frame_id, unsigned int part_id, const char* data, unsigned int part_length);

	sockaddr_in get_sockaddr_in() const;

private:
	unsigned int id;
	std::string token;
	sockaddr_in sin;
	const Clients& clients;
};

class Clients {
public:
	Clients(int& server_socket);

	~Clients();
	
	std::optional<std::reference_wrapper<Client>> register_client(sockaddr_in sin, std::string access_code);

	std::optional<std::reference_wrapper<const Client>> find_client(unsigned int id) const;

	std::optional<std::reference_wrapper<Client>> login_client(unsigned int id, std::string token);

	const int& get_server_socket() const;

private:
	int& server_socket;
	std::vector<Client> clients;
};