#include "App.h"
#include <cassert>


int worker_id;

App::App() {
	server_socket = socket(AF_INET, SOCK_DGRAM, 0);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(62017);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	
	assert(("Bind failed", 0 == bind(server_socket, (sockaddr*) &sin, sizeof(sin))));
}

App::~App() {
	
}

void App::start() {
	std::vector<std::unique_ptr<Worker> > workers;
	for (int i = 0; i < 10; i++) {
		workers.push_back(std::make_unique<Worker>());
	}
}


void worker_thread(Worker* worker, std::shared_ptr<Channel> channel) {
	while (true) {
		Message msg = channel->recv();
		switch (msg.type) {
		case MessageType::STOP:
			return;
		}
	}
}

Worker::Worker() : id(worker_id++) {
	channel = std::make_shared<Channel>();
	thread = std::unique_ptr<std::thread>(new std::thread(worker_thread, this, channel));
}

Worker::~Worker() {
	if (state.get() != nullptr) {
		state->destroy(*this);
	}
	if (thread.get() != nullptr) {
		channel->send(Message(MessageType::STOP));
		thread->join();
	}
}

void Worker::switch_state(std::unique_ptr<State> new_state) {
	if (state.get() != nullptr) {
		state->destroy(*this);
	}
	state.reset(new_state.release());
	state->init(*this);
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
