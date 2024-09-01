#include "SignOut.h"

#include "SignInConst.h"

#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/asyncrt_utils.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;
using namespace utility::conversions;

bool Google::requestSignOut(std::wstring accessToken)
{
	http_client signOutClient(to_string_t(signOutURI));

	auto signOutURL = uri_builder()
		.append_query(U("token"), accessToken);

	http_request signOutReq(methods::POST);
	signOutReq.set_request_uri(signOutURL.to_string());
	signOutReq.headers().set_content_type(L"application/x-www-form-urlencoded");
	auto resp = signOutClient.request(signOutReq).get();

	return (resp.status_code() == status_codes::OK);
}

bool Facebook::requestSignOut(std::wstring accessToken)
{
	http_client signOutClient(to_string_t(signOutURI));

	auto signOutURL = uri_builder()
		.append_query(U("access_token"), accessToken);

	http_request signOutReq(methods::DEL);
	signOutReq.set_request_uri(signOutURL.to_uri());
	signOutReq.headers().set_content_type(L"application/x-www-form-urlencoded");
	auto resp = signOutClient.request(signOutReq).get();

	return (resp.status_code() == status_codes::OK);
}