/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/transport/protocol.hpp>

#include <string>
#include <thread>

#include <gtest/gtest.h>
#include <boost/asio.hpp>

#include <unistd.h>

using namespace vist::transport;
using boost::asio::local::stream_protocol;

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

TEST(ProtocolTests, sync_server_sync_client)
{
	std::string sockPath = "vist-test.sock";
	::unlink(sockPath.c_str());

	/// Server configuration
	boost::asio::io_service context;
	stream_protocol::acceptor acceptor(context, stream_protocol::endpoint(sockPath));
	stream_protocol::socket sock(context);

	auto handler = [&](const auto& error) {
		EXPECT_EQ(error, boost::system::errc::success);

		Message request = Protocol::Recv(sock);
		EXPECT_EQ(request.signature, requestSignature);

		int recv1;
		bool recv2;
		std::string recv3;
		request.disclose(recv1, recv2, recv3);
		EXPECT_EQ(request1, recv1);
		EXPECT_EQ(request2, recv2);
		EXPECT_EQ(request3, recv3);

		Message reply(Message::Type::Reply, responseSignature);
		reply.enclose(response1, response2, response3);
		Protocol::Send(sock, reply);
	};
	acceptor.async_accept(sock, handler);

	auto serverThread = std::thread([&]() {
		context.run(); 
	});

	{ /// Client configuration
		boost::asio::io_service context;
		stream_protocol::socket sock(context);
		sock.connect(stream_protocol::endpoint(sockPath));

		Message request(Message::Type::MethodCall, requestSignature);
		request.enclose(request1, request2, request3);

		auto reply = Protocol::Request(sock, request);
		EXPECT_EQ(reply.signature, responseSignature);

		int recv1;
		bool recv2;
		std::string recv3;
		reply.disclose(recv1, recv2, recv3);
		EXPECT_EQ(response1, recv1);
		EXPECT_EQ(response2, recv2);
		EXPECT_EQ(response3, recv3);
	}

	context.stop();

	if (serverThread.joinable())
		serverThread.join();
}

TEST(ProtocolTests, async_server_sync_client)
{
	std::string sockPath = "vist-test.sock";
	::unlink(sockPath.c_str());

	/// Server configuration
	boost::asio::io_service context;
	stream_protocol::acceptor acceptor(context, stream_protocol::endpoint(sockPath));

	auto async = std::make_shared<Protocol::Async>(context);
	auto handler = [&](const auto& error) {
		EXPECT_EQ(error, boost::system::errc::success);
		auto task = [&](Message message) -> Message {
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

		async->dispatch(task);
	};
	acceptor.async_accept(async->getSocket(), handler);

	auto serverThread = std::thread([&]() {
		context.run(); 
	});

	{ /// Client configuration
		boost::asio::io_service context;
		stream_protocol::socket sock(context);
		sock.connect(stream_protocol::endpoint(sockPath));

		Message request(Message::Type::MethodCall, requestSignature);
		request.enclose(request1, request2, request3);

		auto requestClosure = [&]() {
			auto reply = Protocol::Request(sock, request);
			EXPECT_EQ(reply.signature, responseSignature);

			int recv1;
			bool recv2;
			std::string recv3;
			reply.disclose(recv1, recv2, recv3);
			EXPECT_EQ(response1, recv1);
			EXPECT_EQ(response2, recv2);
			EXPECT_EQ(response3, recv3);
		};

		for (int i = 0; i < 3; i++)
			requestClosure();
	}

	context.stop();

	if (serverThread.joinable())
		serverThread.join();
}
