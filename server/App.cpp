#include "App.h"
#include <cassert>


int worker_id;

App::App() {
	server_socket = socket(AF_INET, SOCK_DGRAM, 0);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(62017);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	
	running = true;

	assert(("Bind failed", 0 == bind(server_socket, (sockaddr*) &sin, sizeof(sin))));
}

App::~App() {
	
}

void App::start() {
	std::shared_ptr<Channel> channel = std::make_shared<Channel>();
	std::vector<std::unique_ptr<Worker> > workers;
	for (int i = 0; i < SERVER_WORKER_SIZE; i++) {
		workers.push_back(std::make_unique<Worker>(channel));
	}
	for (int i = 0; i < workers.size(); i++) {
		channel->send(Message(MessageType::STOP));
	}
}


void worker_thread(Worker* worker) {
	while (true) {
		Message msg = worker->get_channel().recv();
		int* socks = nullptr;
		switch (msg.type) {
		case MessageType::STOP:
			return;

		case MessageType::START:
			break;

		default:
			assert(("Unknown message", false));
			break;
		}

		
	}
}

Worker::Worker(std::shared_ptr<Channel> channel) : id(worker_id++), channel(channel) {
	thread = std::unique_ptr<std::thread>(new std::thread(worker_thread, this));
}

Worker::~Worker() {
	if (thread.get() != nullptr) {
		thread->join();
	}
}

Channel& Worker::get_channel() {
	return *channel.get();
}


Channel::Channel() {
}

Channel::~Channel() {
}

void Channel::send(Message msg) {
	std::unique_lock<std::mutex> lock(mutex);
	messages.push_back(msg);
	lock.unlock();
	cond_var.notify_one();
}

Message Channel::recv() {
	std::unique_lock<std::mutex> lock(mutex);
	cond_var.wait(lock, [&] {
		return messages.size() > 0;
	});
	Message ret = messages[0];
	messages.erase(messages.begin(), messages.begin() + 1);
	return ret;
}

std::optional<Message> Channel::recv_noblock() {
	std::unique_lock<std::mutex> lock(mutex);
	if (messages.size() == 0) {
		return {};
	}
	Message ret = messages[0];
	messages.erase(messages.begin(), messages.begin() + 1);
	return ret;
}


Message::Message(MessageType type) : type(type), data(nullptr) {
}

Message::Message(MessageType type, void* data) : type(type), data(data) {
}
