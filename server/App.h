#pragma once

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <thread>
#include <WinSock2.h>
#include <optional>
#include <thread>
#include <mutex>
#include "Packet.h"
#include "Client.h"
#include "Command.h"

#define SERVER_WORKER_SIZE 10


struct AppData {
	AppData();

	~AppData() = default;

	bool running;
	Clients clients;
	std::mutex mutex;
	int server_socket;
};

class App {
public:
	App();

	~App();

	void start();

private:
	std::vector<std::thread> pool;
	AppData data;
};