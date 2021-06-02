#pragma once


enum class Command {
	NOP = 0,

	CLIENTS_REGISTER,
	CLIENT_PAIR,
	NEW_FRAME,
	FRAME
};

enum class ClientCommand {
	NOP = 0,

	SET_TOKEN,
	NEW_FRAME,
	FRAME,
	PAIR_SUCCESS,
	PAIR_FAILURE,
	PAIR
};

