#pragma once

#ifdef USERAUTH_EXPORTS
#define USERAUTHDLL __declspec(dllexport)
#else
#define USERAUTHDLL __declspec(dllimport)
#endif // USERAUTH_EXPORTS

#include "UserProfile.h"

#include <thread>
#include <string>

enum class SIGNIN_WEB
{
	GOOGLE,
	FACEBOOK
};

class SignIn final
{
public:
	using ACCESS_TOKEN = std::wstring;

	USERAUTHDLL SignIn();
	USERAUTHDLL ~SignIn();

public:
	USERAUTHDLL ACCESS_TOKEN requestSignIn(SIGNIN_WEB webVal);
	
	USERAUTHDLL bool requestSignOut(SIGNIN_WEB webVal, ACCESS_TOKEN token);

	USERAUTHDLL bool checkSignIn(SIGNIN_WEB webVal, ACCESS_TOKEN token);

	USERAUTHDLL UserProfile getUserProfile(SIGNIN_WEB webVal, ACCESS_TOKEN token);

	USERAUTHDLL void openProfileView(SIGNIN_WEB webVal, ACCESS_TOKEN token);

private:
	std::thread googleRedirect;
	std::thread facebookRedirect;
};