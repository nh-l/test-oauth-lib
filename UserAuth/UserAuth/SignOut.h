#pragma once

#include <string>

namespace Google
{
	bool requestSignOut(std::wstring accessToken);
}

namespace Facebook
{
	bool requestSignOut(std::wstring accessToken);
}