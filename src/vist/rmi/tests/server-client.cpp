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

#include <vist/rmi/server.hpp>
#include <vist/rmi/client.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <gtest/gtest.h>

using namespace vist::rmi;
using namespace vist::transport;

// Server side methods
struct Foo {
	bool setName(const std::string& name)
	{
		this->name = name;
		return false;
	}

	std::string getName(void)
	{
		return this->name;
	}

	std::string name;
};

TEST(RmiTests, server_client)
{
	std::string sockPath = ("/tmp/test-server");

	// server-side
	Server server;
	server.listen(sockPath);

	auto foo = std::make_shared<Foo>();
	server.expose(foo, "Foo::setName", &Foo::setName);
	server.expose(foo, "Foo::getName", &Foo::getName);

	auto client = std::thread([&]() {
		std::this_thread::sleep_for(std::chrono::seconds(1));

		// client-side
		Client client(sockPath);

		std::string param = "RMI-TEST";
		bool ret = client.invoke<bool>("Foo::setName", param);
		EXPECT_EQ(ret, false);

		std::string name = client.invoke<std::string>("Foo::getName");
		EXPECT_EQ(name, param);

		server.stop();
	});

	server.start();

	if (client.joinable())
		client.join();
}
