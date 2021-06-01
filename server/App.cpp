#include "App.h"
#include <WS2tcpip.h>

#define TRY_DROP(x, err) if (!x.has_value()) { std::lock_guard<std::mutex> lock(data.mutex); std::cout << "[ERR] " << ip << ": " << err << std::endl; continue; }


App::App() {
	data.server_socket = socket(AF_INET, SOCK_DGRAM, 0);

	sockaddr_in sin;
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	sin.sin_port = htons(60101);
	sin.sin_family = AF_INET;

	assert(bind(data.server_socket, (sockaddr*) &sin, sizeof(sin)) == 0);
}

App::~App() {
	// Is this safe, since this happens after ~AppData() is called?
	closesocket(data.server_socket);
	std::cout << "Waiting for threads to stop..." << std::endl;
	for (unsigned int i = 0; i < pool.size(); i++) {
		pool[i].join();
	}
}

void worker(int id, std::reference_wrapper<AppData> ref_data) {
	AppData& data = ref_data.get();
	{
		std::lock_guard<std::mutex> lock(data.mutex);
		std::cout << "Worker " << id << " launched." << std::endl;
	}
	char payload[MAXIMUM_PAYLOAD_SIZE] = { 0 };
	char ip[16] = { 0 };
	sockaddr_in from;
	int from_len = sizeof(from);
	while (data.running) {
		int size = recvfrom(data.server_socket, payload, sizeof(payload), 0, (sockaddr*) &from, &from_len);
		inet_ntop(AF_INET, (in_addr*) &from.sin_addr, ip, sizeof(ip));
		Packet packet(payload, size);
	
		auto cmd = packet.next<Command>();
		TRY_DROP(cmd, "Command is always needed");

		switch (cmd.value()) {
		case Command::NOP:
			// Do ABSOLUTELY NOTHING
			break;

		case Command::CLIENTS_REGISTER: {
			auto key_length = packet.next<unsigned int>();
			TRY_DROP(key_length, "key_length is not provided");
			auto key = packet.next_ptr<char>(key_length.value());
			TRY_DROP(key, "key is not provided");

			std::stringstream key_stream;
			key_stream.write(key.value(), key_length.value());
			auto ref_client = data.clients.register_client(key_stream.str());
			TRY_DROP(ref_client, "cannot register for some reason");

			Client& client = ref_client.value().get();
			std::string token = client.get_token();
			PacketStream stream;
			stream << ClientCommand::SET_TOKEN;
			stream.write(token.c_str(), token.size());
			std::cout << "Client #" << client.get_id() << " registered. Token: " << token << std::endl;

			std::string str = stream.get_string();
			sendto(data.server_socket, str.c_str(), str.size(), 0, (sockaddr*) &from, from_len);
			break;
		}

		}
	}
}

void App::start() {
	for (int i = 1; i < SERVER_WORKER_SIZE; i++) {
		std::thread worker_thread(worker, i, std::reference_wrapper<AppData>(data));
		pool.push_back(std::move(worker_thread));
	}
	worker(0, data);
}

AppData::AppData() : server_socket(-1), running(true) {

}