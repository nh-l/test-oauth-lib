#pragma once

#include <thread>

#include <cpprest/asyncrt_utils.h>

namespace Server
{
	using USER_STATE = utility::string_t;
	using USER_AUTH_CODE = utility::string_t;

	USER_AUTH_CODE getAuthCode(USER_STATE);
	void waitAuthCode(Server::USER_STATE state);
}

namespace Google
{
	void startRedirectServer();
	void terminateServer();
}

namespace Facebook
{
	void startRedirectServer();
	void terminateServer();
}
