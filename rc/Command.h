#pragma once

#define CLIENT_TOKEN_LEN 8
#define ACCESS_CODE "access_code"


enum class Command {
	NOP = 0,

	CLIENTS_REGISTER,
	CLIENT_PAIR,
	FRAME
};

enum class ClientCommand {
	NOP = 0,

	SET_TOKEN,
	FRAME,
	PAIR_SUCCESS,
	PAIR_FAILURE,
	PAIR
};
