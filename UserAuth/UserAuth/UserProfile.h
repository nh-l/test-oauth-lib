#pragma once

#include <string>

struct UserProfile
{
	std::wstring email;
	std::wstring username;
	std::wstring profileImage;
};

namespace Google
{
	using ACCESS_TOKEN = std::wstring;

	UserProfile getUserProfile(ACCESS_TOKEN token);
}

namespace Facebook
{
	using ACCESS_TOKEN = std::wstring;

	UserProfile getUserProfile(ACCESS_TOKEN token);
}