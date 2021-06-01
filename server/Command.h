#pragma once


enum class Command {
	NOP = 0,

	CLIENTS_REGISTER
};

enum class ClientCommand {
	NOP = 0,

	SET_TOKEN,
	NEW_FRAME,
	FRAME,
};

