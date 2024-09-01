#include "SignIn.h"

#include "LocalServer.h"
#include "ProfileWindow.h"
#include "SignInConst.h"
#include "SignOut.h"

#include "Windows.h"

#include <random>

#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/asyncrt_utils.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;
using namespace utility::conversions;

namespace
{
	std::wstring fbAppAccessToken()
	{
		return to_string_t(Facebook::clientId + "|" + Facebook::clientSecret);
	}

	std::string getAuthURI(SIGNIN_WEB web)
	{
		std::string ret{};
		switch (web)
		{
		case SIGNIN_WEB::GOOGLE:
		{
			ret = Google::authURI;
		}
		break;
		case SIGNIN_WEB::FACEBOOK:
		{
			ret = Facebook::authURI;
		}
		break;
		}

		return ret;
	}

	std::string getRedirectURI(SIGNIN_WEB web)
	{
		std::string ret{};
		switch (web)
		{
		case SIGNIN_WEB::GOOGLE:
		{
			ret = Google::redirectURI;
		}
		break;
		case SIGNIN_WEB::FACEBOOK:
		{
			ret = Facebook::redirectURI;
		}
		break;
		}

		return ret;
	}

	std::string getScope(SIGNIN_WEB web)
	{
		std::string ret{};
		switch (web)
		{
		case SIGNIN_WEB::GOOGLE:
		{
			ret = Google::scope;
		}
		break;
		case SIGNIN_WEB::FACEBOOK:
		{
			ret = Facebook::scope;
		}
		break;
		}

		return ret;
	}

	std::string getClientID(SIGNIN_WEB web)
	{
		std::string ret{};
		switch (web)
		{
		case SIGNIN_WEB::GOOGLE:
		{
			ret = Google::clientId;
		}
		break;
		case SIGNIN_WEB::FACEBOOK:
		{
			ret = Facebook::clientId;
		}
		break;
		}

		return ret;
	}

	std::string getTokenURI(SIGNIN_WEB web)
	{
		std::string ret{};
		switch (web)
		{
		case SIGNIN_WEB::GOOGLE:
		{
			ret = Google::tokenURI;
		}
		break;
		case SIGNIN_WEB::FACEBOOK:
		{
			ret = Facebook::tokenURI;
		}
		break;
		}

		return ret;
	}

	std::string getClientSecret(SIGNIN_WEB web)
	{
		std::string ret{};
		switch (web)
		{
		case SIGNIN_WEB::GOOGLE:
		{
			ret = Google::clientSecret;
		}
		break;
		case SIGNIN_WEB::FACEBOOK:
		{
			ret = Facebook::clientSecret;
		}
		break;
		}

		return ret;
	}

	std::string getCheckURI(SIGNIN_WEB web)
	{
		std::string ret{};
		switch (web)
		{
		case SIGNIN_WEB::GOOGLE:
		{
			ret = Google::checkURI;
		}
		break;
		case SIGNIN_WEB::FACEBOOK:
		{
			ret = Facebook::checkURI;
		}
		break;
		}

		return ret;
	}
	
	std::string generateState()
	{
		const int len = 32;
		const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::default_random_engine generator(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, sizeof(charset) - 2);

		std::string state;
		for (int i = 0; i < len; ++i) {
			state += charset[dist(generator)];
		}
		
		return state;
	}
	
	uri getCheckURL(SIGNIN_WEB web, SignIn::ACCESS_TOKEN token)
	{
		uri_builder ret;
		switch (web)
		{
		case SIGNIN_WEB::GOOGLE:
		{
			ret = uri_builder().append_query(U("access_token"), token);
		}
		break;
		case SIGNIN_WEB::FACEBOOK:
		{
			ret = uri_builder()
				.append_query(U("input_token"), token)
				.append_query(U("access_token"), fbAppAccessToken());
		}
		break;
		}

		return ret.to_uri();
	}
	
	method getHTTPMethods(SIGNIN_WEB web)
	{
		method ret;
		switch (web)
		{
		case SIGNIN_WEB::GOOGLE:
		{
			ret = methods::POST;
		}
		break;
		case SIGNIN_WEB::FACEBOOK:
		{
			ret = methods::GET;
		}
		break;
		}

		return ret;
	}

	SignIn::ACCESS_TOKEN getAccessToken(SIGNIN_WEB web, std::string state)
	{
		//액세스 토큰 요청
		http_client tokenClient(to_string_t(getTokenURI(web)));

		auto tokenURL = uri_builder()
			.append_query(U("code"), Server::getAuthCode(to_string_t(state)))
			.append_query(U("redirect_uri"), to_string_t(getRedirectURI(web)))
			.append_query(U("client_id"), to_string_t(getClientID(web)))
			.append_query(U("client_secret"), to_string_t(getClientSecret(web)))
			.append_query(U("grant_type"), U("authorization_code"));

		http_request tokenReq(getHTTPMethods(web));
		tokenReq.set_request_uri(tokenURL.to_string());
		tokenReq.headers().set_content_type(L"application/x-www-form-urlencoded");
		auto resp = tokenClient.request(tokenReq).get();
		auto jsonresp = resp.extract_json().get();

		if (jsonresp.has_field(L"access_token")) {
			return jsonresp[L"access_token"].as_string();
		}

		return std::wstring{};
	}

	void printJSONObject(http_response& resp)
	{
		auto jsonObj = resp.extract_json().get();
		for (const auto& item : jsonObj.as_object())
		{
			std::wcout << item.first << " , " << item.second << std::endl;
		}
	}
}

SignIn::SignIn()
	: googleRedirect(Google::startRedirectServer),
	facebookRedirect(Facebook::startRedirectServer)
{
	PW::initialize();
}

SignIn::~SignIn()
{
	PW::shutDown();

	Google::terminateServer();
	if (googleRedirect.joinable())
	{
		googleRedirect.join();
	}

	Facebook::terminateServer();
	if (facebookRedirect.joinable())
	{
		facebookRedirect.join();
	}
}

SignIn::ACCESS_TOKEN SignIn::requestSignIn(SIGNIN_WEB webVal)
{
	auto state = generateState();
	//사용자 인증 요청 URL 생성
	auto requestURL = uri_builder(to_string_t(getAuthURI(webVal)))
		.append_query(U("response_type"), U("code"))
		.append_query(U("redirect_uri"), to_string_t(getRedirectURI(webVal)))
		.append_query(U("scope"), to_string_t(getScope(webVal)))
		.append_query(U("client_id"), to_string_t(getClientID(webVal)))
		.append_query(U("state"), to_string_t(state))
		.to_uri();

	//로그인 페이지 열기
	ShellExecuteW(NULL, L"open", requestURL.to_string().c_str(), NULL, NULL, SW_SHOWNORMAL);

	Server::waitAuthCode(to_string_t(state));
	
	return getAccessToken(webVal, state);
}

bool SignIn::requestSignOut(SIGNIN_WEB webVal, SignIn::ACCESS_TOKEN token)
{
	bool ret = false;

	switch (webVal)
	{
	case SIGNIN_WEB::GOOGLE:
	{
		ret = Google::requestSignOut(token);
	}
	break;
	case SIGNIN_WEB::FACEBOOK:
	{
		ret = Facebook::requestSignOut(token);
	}
	break;
	}

	return ret;
}

bool SignIn::checkSignIn(SIGNIN_WEB webVal, SignIn::ACCESS_TOKEN token)
{
	http_client checkClient(to_string_t(getCheckURI(webVal)));

	http_request checkReq(methods::GET);
	checkReq.set_request_uri(getCheckURL(webVal, token));
	checkReq.headers().set_content_type(L"application/x-www-form-urlencoded");

	auto resp = checkClient.request(checkReq).get();

	return (resp.status_code() == status_codes::OK);
}

UserProfile SignIn::getUserProfile(SIGNIN_WEB webVal, ACCESS_TOKEN token)
{
	UserProfile profile;
	switch (webVal)
	{
	case SIGNIN_WEB::GOOGLE:
	{
		profile = Google::getUserProfile(token);
	}
	break;
	case SIGNIN_WEB::FACEBOOK:
	{
		profile = Facebook::getUserProfile(token);
	}
	break;
	}

	return profile;
}

void SignIn::openProfileView(SIGNIN_WEB webVal, ACCESS_TOKEN token)
{
	if (checkSignIn(webVal, token))
	{
		PW::setUserProfile(getUserProfile(webVal, token));
	}

	WNDCLASS wc{};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"ProfileWindowClass";
	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"UserAuth App",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, 
		NULL, NULL, GetModuleHandle(NULL), NULL);

	if (hwnd)
	{
		ShowWindow(hwnd, SW_SHOW);

		MSG msg{};
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}