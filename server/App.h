#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <WinSock2.h>
#include <optional>
#include <mutex>
#include <condition_variable>

#define SERVER_WORKER_SIZE 10
#define MAXIMUM_PAYLOAD_SIZE 508


enum class MessageType {
	STOP,
	START // Data should contain two sockets
};

struct Message {
	Message(MessageType type);

	Message(MessageType type, void* data);

	MessageType type;
	void* data;
};

class Channel {
public:
	Channel();

	~Channel();

	void send(Message msg);

	Message recv();

	std::optional<Message> recv_noblock();

private:
	std::mutex mutex;
	std::condition_variable cond_var;
	std::vector<Message> messages;
};

class Worker {
public:
	Worker(std::shared_ptr<Channel> channel);

	~Worker();

	Channel& get_channel();

private:
	std::shared_ptr<Channel> channel;
	std::unique_ptr<std::thread> thread;
	int id;
};

class App {
public:
	App();

	~App();

	void start();

private:
	bool running;
	int server_socket;
	std::vector<std::unique_ptr<Worker> > worker_pool;
};

