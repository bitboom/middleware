/*
 *  Copyright (c) 2020 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/rmi/impl/socket.hpp>
#include <vist/rmi/impl/mainloop.hpp>

#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

using namespace vist::rmi;
using namespace vist::rmi::impl;

TEST(MainloopTests, single)
{
	std::string sockPath = "@sock";
	Socket socket(sockPath);
	Mainloop mainloop;

	int input = std::numeric_limits<int>::max();
	bool input2 = true;

	int output = 0;
	bool output2 = false;

	auto onAccept = [&]() {
		Socket accepted = socket.accept();

		// Recv input from client.
		accepted.recv(&output);
		EXPECT_EQ(input, output);

		// Send input2 to client.
		accepted.send(&input2);

		mainloop.removeHandler(socket.getFd());
		mainloop.stop();
	};

	mainloop.addHandler(socket.getFd(), std::move(onAccept));
	auto server = std::thread([&]() { mainloop.run(); });

	// Send input to server.
	Socket connected = Socket::connect(sockPath);
	connected.send(&input);

	// Recv input2 from server.
	connected.recv(&output2);
	EXPECT_EQ(input2, output2);

	if (server.joinable())
		server.join();
}

TEST(MainloopTests, multiflexing)
{
	std::string sockPath = "@sock";
	Socket socket(sockPath);
	Mainloop mainloop;

	int input = std::numeric_limits<int>::max();
	bool input2 = true;

	int output = 0;
	bool output2 = false;

	auto onAccept = [&]() {
		Socket accepted = socket.accept();

		// Recv input from client.
		accepted.recv(&output);
		EXPECT_EQ(input, output);

		// Send input2 to client.
		accepted.send(&input2);
	};

	/// Set timeout to stop
	mainloop.addHandler(socket.getFd(), std::move(onAccept));
	auto server = std::thread([&]() { mainloop.run(1000); });

	auto task = [&]() {
		// Send input to server.
		Socket connected = Socket::connect(sockPath);
		connected.send(&input);

		// Recv input2 from server.
		connected.recv(&output2);
		EXPECT_EQ(input2, output2);
	};

	std::vector<std::thread> clients;
	clients.emplace_back(std::thread(task));
	clients.emplace_back(std::thread(task));
	clients.emplace_back(std::thread(task));

	if (server.joinable())
		server.join();

	for (auto& client : clients)
		if (client.joinable())
			client.join();
}

TEST(MainloopTests, stopper)
{
	auto stopper = []() -> bool { return true; };
	Mainloop mainloop;
	mainloop.run(1000, stopper);
	EXPECT_TRUE(true);

	mainloop.run(1000);
	EXPECT_TRUE(true);
}
