/*
 *  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */

#include <vist/rmi/message.hpp>
#include <vist/rmi/impl/server.hpp>
#include <vist/rmi/impl/client.hpp>

#include <string>
#include <thread>

#include <gtest/gtest.h>

using namespace vist::rmi;
using namespace vist::rmi::impl;

namespace {

/// Request variables
std::string requestSignature = "request signature";
int request1 = 100;
bool request2 = true;
std::string request3 = "request argument";

/// Response variables
std::string responseSignature = "response signature";
int response1 = 300;
bool response2 = false;
std::string response3 = "response argument";

} // anonymous namespace

TEST(ServerClientTests, not_ondemand)
{
	std::string sockPath = "@vist-test.sock";

	auto task = [&](Message& message) -> Message {
		EXPECT_EQ(message.signature, requestSignature);

		int recv1;
		bool recv2;
		std::string recv3;
		message.disclose(recv1, recv2, recv3);
		EXPECT_EQ(request1, recv1);
		EXPECT_EQ(request2, recv2);
		EXPECT_EQ(request3, recv3);

		Message reply(Message::Type::Reply, responseSignature);
		reply.enclose(response1, response2, response3);
		return reply;
	};

	Server server(sockPath, task);
	auto serverThread = std::thread([&]() {
		server.run();
	});

	{ /// Client configuration
		auto clientClosure = [&]() {
			Client client(sockPath);

			Message message(Message::Type::MethodCall, requestSignature);
			message.enclose(request1, request2, request3);

			for (int i = 0; i < 3; i++) {
				auto reply = client.request(message);
				EXPECT_EQ(reply.signature, responseSignature);

				int recv1;
				bool recv2;
				std::string recv3;
				reply.disclose(recv1, recv2, recv3);
				EXPECT_EQ(response1, recv1);
				EXPECT_EQ(response2, recv2);
				EXPECT_EQ(response3, recv3);
			}
		};

		for (int i = 0; i < 3; i++)
			clientClosure();
	}

	server.stop();

	if (serverThread.joinable())
		serverThread.join();
}

TEST(ServerClientTests, peer_pid)
{
	std::string sockPath = "@vist-test.sock";
	auto task = [](Message& message) -> Message {
		EXPECT_EQ(message.signature, requestSignature);

		auto peer = Server::GetPeerCredentials();
		EXPECT_TRUE(peer != nullptr);

		int recv1;
		message.disclose(recv1);
		EXPECT_EQ(request1, recv1);

		Message reply(Message::Type::Reply, responseSignature);
		reply.enclose(response1);
		return reply;
	};

	Server server(sockPath, task);
	auto serverThread = std::thread([&]() {
		server.run();
	});

	{ /// Client configuration
		auto clientClosure = [&]() {
			Client client(sockPath);

			Message message(Message::Type::MethodCall, requestSignature);
			message.enclose(request1);

			auto reply = client.request(message);
			EXPECT_EQ(reply.signature, responseSignature);

			int recv1;
			reply.disclose(recv1);
			EXPECT_EQ(response1, recv1);
		};

		clientClosure();
	}

	server.stop();

	if (serverThread.joinable())
		serverThread.join();
}
