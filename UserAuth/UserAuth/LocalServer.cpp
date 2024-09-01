#include "LocalServer.h"

#include "SignInConst.h"

#include <condition_variable>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

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
	std::map<Server::USER_STATE, Server::USER_AUTH_CODE> codeList;
	std::map<Server::USER_STATE, std::pair<std::condition_variable, std::mutex>> condVarList;

	std::mutex codeListLock;
	std::mutex notifyCodeLock;
	
	void addCodeList(Server::USER_STATE state, Server::USER_AUTH_CODE code)
	{
		std::unique_lock<std::mutex> lock(codeListLock);

		codeList[state] = code;
	}
	
	void notifyGetAuthCode(Server::USER_STATE state)
	{
		std::unique_lock<std::mutex> lock(notifyCodeLock);

		auto iter = condVarList.find(state);
		if (iter != condVarList.end())
		{
			auto& [cv, mtx] = iter->second;
			std::unique_lock<std::mutex> cvLock(mtx);
			cv.notify_one();
		}
	}

	void startServer(std::wstring redirectURI, std::mutex& serverLock,
		std::condition_variable& serverCondVar)
	{
		std::unique_lock<std::mutex> lock(serverLock);

		http_listener listener(uri_builder(redirectURI).to_uri());

		listener.support(methods::GET, [](http_request req) {
			auto queries = uri::split_query(req.request_uri().query());
			auto code_iter = queries.find(U("code"));
			auto state_iter = queries.find(U("state"));

			if (code_iter != queries.end() && state_iter != queries.end()) {
				addCodeList(state_iter->second, uri::decode(code_iter->second));
				notifyGetAuthCode(state_iter->second);

				req.reply(status_codes::OK, "close this browser.");
			}
			else {
				req.reply(status_codes::BadRequest, "error!!!");
			}
			});

		listener.open().wait();

		serverCondVar.wait(lock);

		listener.close();
	}
}

Server::USER_AUTH_CODE Server::getAuthCode(Server::USER_STATE state)
{
	std::unique_lock<std::mutex> lock(codeListLock);

	return codeList[state];
}

void Server::waitAuthCode(Server::USER_STATE state)
{
	std::unique_lock<std::mutex> lock(notifyCodeLock);
	auto& [cv, mtx] = condVarList[state];
	lock.unlock();

	std::unique_lock<std::mutex> cvLock(mtx);
	cv.wait(cvLock);
}

namespace Google
{
	std::condition_variable googleCV;

	void startRedirectServer()
	{
		std::mutex googleMutex;
		std::thread googleRedirect(startServer, to_string_t(redirectURI), std::ref(googleMutex), std::ref(googleCV));

		googleRedirect.join();
	}

	void terminateServer()
	{
		googleCV.notify_one();
	}
}

namespace Facebook
{
	std::condition_variable facebookCV;

	void startRedirectServer()
	{
		std::mutex fbMutex;
		std::thread fbRedirect(startServer, to_string_t(redirectURI), std::ref(fbMutex), std::ref(facebookCV));

		fbRedirect.join();
	}

	void terminateServer()
	{
		facebookCV.notify_one();
	}
}
