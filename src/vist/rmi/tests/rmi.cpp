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

#include <vist/rmi/gateway.hpp>
#include <vist/rmi/remote.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <gtest/gtest.h>

using namespace vist::rmi;

// gateway side methods
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

struct Bar {
	int plusTwo(int number)
	{
		return number + 2;
	}
};

TEST(RmiTests, positive)
{
	std::string sockPath = ("/tmp/test-gateway");

	// gateway-side
	Gateway gateway(sockPath, Gateway::ServiceType::General);

	Foo foo;
	gateway.expose(foo, "Foo::setName", &Foo::setName);
	gateway.expose(foo, "Foo::getName", &Foo::getName);

	// convenience macro
	Bar bar;
	EXPOSE(gateway, bar, &Bar::plusTwo);

	auto client = std::thread([&]() {
		// caller-side
		Remote remote(sockPath);

		std::string param = "RMI-TEST";
		bool ret = remote.invoke<bool>("Foo::setName", param);
		EXPECT_EQ(ret, false);

		std::string name = remote.invoke<std::string>("Foo::getName");
		EXPECT_EQ(name, param);

		// convenience macro
		auto plusTwo = REMOTE_METHOD(remote, &Bar::plusTwo);
		int num = plusTwo.invoke<int>(3);
		EXPECT_EQ(num, 5);

		gateway.stop();
	});

	gateway.start();

	if (client.joinable())
		client.join();
}

TEST(RmiTests, not_exist_method)
{
	std::string sockPath = ("/tmp/test-gateway");

	// gateway-side
	Gateway gateway(sockPath);

	auto client = std::thread([&]() {
		// caller-side
		Remote remote(sockPath);

		std::string param = "RMI-TEST";

		bool rasied = false;
		try {
			remote.invoke<bool>("Foo::NotExist", param);
		} catch (const std::exception& e) {
			rasied = true;
		}
		EXPECT_TRUE(rasied);

		gateway.stop();
	});

	gateway.start();

	if (client.joinable())
		client.join();
}
