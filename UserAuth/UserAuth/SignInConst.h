#pragma once

#include <string>

namespace Google
{
	const std::string clientId = "";
	const std::string clientSecret = "";
	const std::string scope = "email profile openid";

	const std::string redirectURI = "http://localhost:3000/cb";
	const std::string authURI = "https://accounts.google.com/o/oauth2/v2/auth";
	const std::string tokenURI = "https://oauth2.googleapis.com/token";
	const std::string signOutURI = "https://oauth2.googleapis.com/revoke";
	const std::string checkURI = "https://oauth2.googleapis.com/tokeninfo";
	const std::string userInfoURI = "https://openidconnect.googleapis.com/v1/userinfo";
}

namespace Facebook
{
	const std::string clientId = "";
	const std::string clientSecret = "";
	const std::string scope = "email public_profile";

	const std::string facebookURI = "https://www.facebook.com/connect/login_success.html";
	const std::string redirectURI = "http://localhost:3000/fblogin";
	const std::string authURI = "https://www.facebook.com/v20.0/dialog/oauth";
	const std::string appAcessURI = "https://graph.facebook.com/oauth";
	const std::string tokenURI = "https://graph.facebook.com/v20.0/oauth/access_token";
	const std::string checkURI = "https://graph.facebook.com/v20.0/debug_token";
	const std::string userInfoURI = "https://graph.facebook.com/v20.0/me";
	const std::string signOutURI = "https://graph.facebook.com/v20.0/me/permissions";
}