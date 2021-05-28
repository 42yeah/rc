#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <WinSock2.h>
#include <optional>
#include <mutex>
#include <condition_variable>
#include "State.h"

#define SERVER_WORKER_SIZE 10


enum class MessageType {
	STOP
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
	Worker();

	~Worker();

	void switch_state(std::unique_ptr<State> new_state);

	Channel& get_channel();

private:
	std::shared_ptr<Channel> channel;
	std::unique_ptr<std::thread> thread;
	std::unique_ptr<State> state;
	int id;
};

class App {
public:
	App();

	~App();

	void start();

private:
	int server_socket;
	std::vector<std::unique_ptr<Worker> > worker_pool;
};

