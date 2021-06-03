#pragma once

#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <functional>


enum class MessageType {
	STOP = 0,
	EXECUTE
};

struct Message {
	MessageType type;
	std::optional<std::function<void(void)> > job;
};

struct Channel {
public:
	Channel();

	void write(Message msg);

	Message read();

	std::vector<Message> messages;
	std::mutex mutex;
	std::condition_variable cond_var;

	std::mutex busy_mutex;
	std::condition_variable busy_var;
	int busy_counter;
};

class ThreadPool {
public:
	ThreadPool(unsigned int n);

	~ThreadPool();

	void execute(std::function<void(void)> func);

	void wait_until_idle();

private:
	Channel channel;
	std::vector<std::thread> threads;
};

