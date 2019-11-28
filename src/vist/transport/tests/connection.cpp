/*
 *  Copyright (c) 2018-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/transport/connection.hpp>
#include <vist/transport/socket.hpp>
#include <vist/event/mainloop.hpp>
#include <vist/event/eventfd.hpp>

#include <string>
#include <thread>

#include <gtest/gtest.h>

using namespace vist::transport;
using namespace vist::event;

TEST(TransportTests, socket_communication)
{
	std::string sockPath = ("/tmp/vist-test.sock");

	// server-side
	Mainloop mainloop;
	Socket socket(sockPath);

	std::string requestSignature = "request signature";
	int request1 = 100;
	bool request2 = true;
	std::string request3 = "request argument";

	std::string responseSignature = "response signature";
	int response1 = 300;
	bool response2 = false;
	std::string response3 = "response argument";

	auto onAccept = [&]() {
		Connection conn(socket.accept());
		Message request = conn.recv();
		EXPECT_EQ(requestSignature, request.signature);

		int recv1;
		bool recv2;
		std::string recv3;
		request.disclose(recv1, recv2, recv3);
		EXPECT_EQ(request1, recv1);
		EXPECT_EQ(request2, recv2);
		EXPECT_EQ(request3, recv3);

		Message reply(Message::Type::Reply, responseSignature);
		reply.enclose(response1, response2, response3);
		conn.send(reply);

		mainloop.removeHandler(socket.getFd());
		mainloop.stop();
	};

	mainloop.addHandler(socket.getFd(), std::move(onAccept));
	auto serverThread = std::thread([&]() { mainloop.run(); });

	// client-side
	Connection conn(sockPath);
	Message msg(Message::Type::Signal, requestSignature);
	msg.enclose(request1, request2, request3);

	Message reply = conn.request(msg);
	EXPECT_EQ(reply.signature, responseSignature);

	int recv1;
	bool recv2;
	std::string recv3;
	reply.disclose(recv1, recv2, recv3);
	EXPECT_EQ(response1, recv1);
	EXPECT_EQ(response2, recv2);
	EXPECT_EQ(response3, recv3);

	if (serverThread.joinable())
		serverThread.join();
}
