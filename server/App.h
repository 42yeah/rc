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
#include "Client.h"

#define SERVER_WORKER_SIZE 10
#define MAXIMUM_PAYLOAD_SIZE 512


struct AppData {
	AppData();

	~AppData() = default;

	bool running;
	Clients clients;
	std::mutex mutex;
	int server_socket;
};

enum class Command {
	NOP = 0,

	CLIENTS_REGISTER
};

enum class ClientCommand {
	NOP = 0,

	SET_TOKEN = 1
};

struct Packet {
	Packet(char* ptr, unsigned int size) : ptr(ptr), size(size), current_offset(0) {}

	template<typename T>
	std::optional<T> next() {
		if (current_offset + sizeof(T) > size) {
			return {};
		}
		int old_offset = current_offset;
		current_offset += sizeof(T);
		return *(T*) &ptr[old_offset];
	}

	template<typename T>
	std::optional<T*> next_ptr(unsigned int buf_size) {
		if (current_offset + buf_size > size) {
			return {};
		}
		int old_offset = current_offset;
		current_offset += buf_size;
		return (T*) &ptr[old_offset];
	}

	char* ptr;
	unsigned int size;
	int current_offset;
};

class PacketStream {
public:
	PacketStream() : size(0) {}

	template<typename T>
	PacketStream &operator<<(T what) {
		assert(size + sizeof(T) <= MAXIMUM_PAYLOAD_SIZE);
		size += sizeof(T);
		stream.write((const char*) &what, sizeof(T));
		return *this;
	}

	void write(const char* what, unsigned int size) {
		assert(this->size + size <= MAXIMUM_PAYLOAD_SIZE);
		this->size += size;
		stream.write(what, size);
	}

	std::string get_string() {
		return stream.str();
	}

private:
	std::stringstream stream;
	unsigned int size;
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