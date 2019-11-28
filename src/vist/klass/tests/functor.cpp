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

#include <vist/archive.hpp>
#include <vist/klass/functor.hpp>

#include <iostream>
#include <memory>

#include <gtest/gtest.h>

using namespace vist;
using namespace vist::klass;

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

	int echo(const std::string& a, const std::string b, std::string& c)
	{
		std::cout << a << ", " << b << ", " << c << std::endl;
		return false;
	}

//	void impossible(void) {}

	std::string name;
};

TEST(FunctorTests, functor)
{
	auto foo = std::make_shared<Foo>();
	auto fooSetName = make_functor(foo, &Foo::setName);
	auto fooGetName = make_functor(foo, &Foo::getName);
	auto fooEcho = make_functor(foo, &Foo::echo);
//	auto fooImp = make_functor(foo, &Foo::impossible);

	std::string input = "Foo name";
	bool ret = true;
	ret = fooSetName(input);
	EXPECT_EQ(ret, false);

	std::string output = fooGetName();
	EXPECT_EQ(output, input);

	std::string a("aaa"), b("bbb"), c("ccc");
	ret = true;
	ret = fooEcho(a, b, c);
	EXPECT_EQ(ret, false);
}

TEST(FunctorTests, functor_map)
{
	auto foo = std::make_shared<Foo>();

	FunctorMap fooMap;
	fooMap["setName"] = make_functor_ptr(foo, &Foo::setName);
	fooMap["getName"] = make_functor_ptr(foo, &Foo::getName);
	fooMap["echo"] = make_functor_ptr(foo, &Foo::echo);

	auto fooSetNamePtr = fooMap.at("setName");
	auto fooGetNamePtr = fooMap.at("getName");
	auto fooEchoPtr = fooMap.at("echo");

	std::string input = "Foo name";
	bool ret = true;
	ret = fooSetNamePtr->invoke<bool>(input);
	EXPECT_EQ(ret, false);

	std::string output = fooGetNamePtr->invoke<std::string>();
	EXPECT_EQ(output, input);

	std::string a("aaaa"), b("bbbb"), c("cccc");
	ret = true;
	ret = fooEchoPtr->invoke<bool>(a, b, c);
	EXPECT_EQ(ret, false);
}

TEST(FunctorTests, archive)
{
	auto foo = std::make_shared<Foo>();

	FunctorMap fooMap;
	fooMap["echo"] = make_functor_ptr(foo, &Foo::echo);

	std::string a("aaaa"), b("bbbb"), c("cccc");
	Archive archive;
	archive << a << b << c;

	auto fooEchoPtr = fooMap.at("echo");
	auto result = fooEchoPtr->invoke(archive);
	bool ret = true;
	result >> ret;
	EXPECT_EQ(ret, false);
}
