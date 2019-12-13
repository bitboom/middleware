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
/*
 * @file   rmi.cpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Testcases of rmi.
 */


#include <vist/rmi/exposer.hpp>
#include <vist/rmi/remote.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <gtest/gtest.h>

using namespace vist::rmi;

// exposer side methods
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
	std::string sockPath = ("/tmp/test-exposer");

	// exposer-side
	Exposer exposer(sockPath);

	auto foo = std::make_shared<Foo>();
	exposer.expose(foo, "Foo::setName", &Foo::setName);
	exposer.expose(foo, "Foo::getName", &Foo::getName);

	auto bar = std::make_shared<Bar>();
	exposer.expose(bar, "Bar::plusTwo", &Bar::plusTwo);

	auto client = std::thread([&]() {
		// caller-side
		Remote remote(sockPath);

		std::string param = "RMI-TEST";
		bool ret = remote.invoke<bool>("Foo::setName", param);
		EXPECT_EQ(ret, false);

		std::string name = remote.invoke<std::string>("Foo::getName");
		EXPECT_EQ(name, param);

		int num = remote.invoke<int>("Bar::plusTwo", 3);
		EXPECT_EQ(num, 5);

		exposer.stop();
	});

	exposer.start();

	if (client.joinable())
		client.join();
}

TEST(RmiTests, not_exist_method)
{
	std::string sockPath = ("/tmp/test-exposer");

	// exposer-side
	Exposer exposer(sockPath);

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

		exposer.stop();
	});

	exposer.start();

	if (client.joinable())
		client.join();
}
