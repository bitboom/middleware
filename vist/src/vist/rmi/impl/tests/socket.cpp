/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/credentials.hpp>
#include <vist/rmi/impl/socket.hpp>

#include <cstring>
#include <limits>
#include <string>
#include <thread>

#include <gtest/gtest.h>

using namespace vist::rmi::impl;

TEST(SocketTests, socket_read_write)
{
	std::string sockPath = "./test.sock";
	Socket socket(sockPath);

	int input = std::numeric_limits<int>::max();
	bool input2 = true;

	int output = 0;
	bool output2 = false;

	auto client = std::thread([&]() {
		// Send input to server.
		Socket connected = Socket::connect(sockPath);
		connected.send(&input);

		// Recv input2 from server.
		connected.recv(&output2);

		EXPECT_EQ(input2, output2);
	});

	Socket accepted = socket.accept();

	// Recv input from client.
	accepted.recv(&output);
	EXPECT_EQ(input, output);

	// Send input2 to client.
	accepted.send(&input2);

	if (client.joinable())
		client.join();
}

TEST(SocketTests, socket_abstract)
{
	std::string sockPath = "@sock";
	Socket socket(sockPath);

	int input = std::numeric_limits<int>::max();
	bool input2 = true;

	int output = 0;
	bool output2 = false;

	auto client = std::thread([&]() {
		// Send input to server.
		Socket connected = Socket::connect(sockPath);
		connected.send(&input);

		// Recv input2 from server.
		connected.recv(&output2);

		EXPECT_EQ(input2, output2);
	});

	Socket accepted = socket.accept();

	// Recv input from client.
	accepted.recv(&output);
	EXPECT_EQ(input, output);

	// Send input2 to client.
	accepted.send(&input2);

	if (client.joinable())
		client.join();
}

TEST(SocketTests, peer_credeintials)
{
	std::string sockPath = "@sock";
	Socket socket(sockPath);

	int input = std::numeric_limits<int>::max();
	auto client = std::thread([&]() {
		Socket connected = Socket::connect(sockPath);
		connected.send(&input);
	});

	Socket accepted = socket.accept();

	auto cred = vist::Credentials::Peer(accepted.getFd());
	EXPECT_TRUE(cred.pid > 0);

	// Recv input from client.
	int output = 0;
	accepted.recv(&output);
	EXPECT_EQ(input, output);

	if (client.joinable())
		client.join();
}
