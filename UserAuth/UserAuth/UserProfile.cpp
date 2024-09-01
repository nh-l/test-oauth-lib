#include "UserProfile.h"

#include "SignInConst.h"

#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/asyncrt_utils.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;
using namespace utility::conversions;

UserProfile Google::getUserProfile(ACCESS_TOKEN token)
{
	http_client profileClient(to_string_t(Google::userInfoURI));
	http_request profileReq(methods::GET);
	profileReq.set_request_uri(uri_builder().append_query(U("access_token"), token).to_string());
	auto resp = profileClient.request(profileReq).get();

	UserProfile result;
	if (resp.status_code() == status_codes::OK)
	{
		auto jsonresp = resp.extract_json().get();

		if (jsonresp.has_field(L"email")) {
			result.email = jsonresp[L"email"].as_string();
		}
		if (jsonresp.has_field(L"name")) {
			result.username = jsonresp[L"name"].as_string();
		}
		if (jsonresp.has_field(L"picture")) {
			result.profileImage = jsonresp[L"picture"].as_string();
		}
	}

	return result;
}

UserProfile Facebook::getUserProfile(ACCESS_TOKEN token)
{
	http_client profileClient(to_string_t(Facebook::userInfoURI));
	http_request profileReq(methods::GET);
	profileReq.set_request_uri(uri_builder()
		.append_query(U("fields"), U("name,email,picture"))
		.append_query(U("access_token"), token).to_uri());
	auto resp = profileClient.request(profileReq).get();

	UserProfile result;
	if (resp.status_code() == status_codes::OK)
	{
		auto jsonresp = resp.extract_json().get();

		if (jsonresp.has_field(L"email")) {
			result.email = jsonresp[L"email"].as_string();
		}
		if (jsonresp.has_field(L"name")) {
			result.username = jsonresp[L"name"].as_string();
		}
		if (jsonresp.has_field(L"picture")) {
			result.profileImage = jsonresp[L"picture"].as_object()[L"data"].as_object()[L"url"].as_string();
		}
	}

	return result;
}