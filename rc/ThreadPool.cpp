#include "ThreadPool.h"
#include <iostream>


void worker(int id, std::reference_wrapper<Channel> ref_channel) {
	Channel& channel = ref_channel.get();

	while (true) {
		Message msg = channel.read();
		{
			std::unique_lock<std::mutex> busy_lock(channel.busy_mutex);
			channel.busy_counter++;
		}
		
		switch (msg.type) {
		case MessageType::STOP:
			return;

		case MessageType::EXECUTE:
			if (msg.job.has_value()) {
				msg.job.value()();
			}
			break;
		}
		{
			std::unique_lock<std::mutex> busy_lock(channel.busy_mutex);
			channel.busy_counter--;
			channel.busy_var.notify_one();
		}
	}
}

ThreadPool::ThreadPool(unsigned int n) {
	for (int i = 0; i < n; i++) {
		threads.push_back(std::thread(worker, i, std::reference_wrapper<Channel>(channel)));
	}
}

ThreadPool::~ThreadPool() {
	for (int i = 0; i < threads.size(); i++) {
		channel.write(Message{ MessageType::STOP, {} });
	}
	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
}

void ThreadPool::execute(std::function<void(void)> func) {
	channel.write(Message{ MessageType::EXECUTE, func });
}

void ThreadPool::wait_until_idle() {
	std::unique_lock<std::mutex> busy_lock(channel.busy_mutex);
	channel.busy_var.wait(busy_lock, [&]() {
		return channel.busy_counter == 0;
	});
}


Channel::Channel() : busy_counter(0) {

}

void Channel::write(Message msg) {
	std::unique_lock<std::mutex> lock(mutex);

	messages.push_back(msg);
	cond_var.notify_one();
}

Message Channel::read() {
	std::unique_lock<std::mutex> lock(mutex);

	cond_var.wait(lock, [&]() {
		return messages.size() > 0;
	});
	Message message = messages[0];
	messages.erase(messages.begin(), messages.begin() + 1);

	return message;
}
