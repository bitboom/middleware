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

#include <gtest/gtest.h>

#include <vist/stringfy.hpp>
#include <vist/exception.hpp>

using namespace vist;

TEST(StringifyTests, integer_dump)
{
	EXPECT_EQ("I/-11", Stringify::Dump(-11));
	EXPECT_EQ("I/-1", Stringify::Dump(-1));
	EXPECT_EQ("I/0", Stringify::Dump(0));
	EXPECT_EQ("I/1", Stringify::Dump(1));
	EXPECT_EQ("I/11", Stringify::Dump(11));
}

TEST(StringifyTests, string_dump)
{
	EXPECT_EQ("S/", Stringify::Dump(""));
	EXPECT_EQ("S/TEXT", Stringify::Dump("TEXT"));
}

TEST(StringifyTests, integer_restore)
{
	auto dumped = Stringify::Dump(-1);
	EXPECT_EQ(-1, Stringify::Restore(dumped));

	dumped = Stringify::Dump(0);
	EXPECT_EQ(0, Stringify::Restore(dumped));

	dumped = Stringify::Dump(1);
	EXPECT_EQ(1, Stringify::Restore(dumped));
}

TEST(StringifyTests, string_restore)
{
	auto dumped = Stringify::Dump("");
	EXPECT_EQ(std::string(""), static_cast<std::string>(Stringify::Restore(dumped)));

	dumped = Stringify::Dump("TEXT");
	EXPECT_EQ(std::string("TEXT"), static_cast<std::string>(Stringify::Restore(dumped)));
}

TEST(StringifyTests, get_type)
{
	auto dumped = Stringify::Dump(0);
	EXPECT_EQ(Stringify::Type::Integer, Stringify::GetType(dumped));

	dumped = Stringify::Dump("Text");
	EXPECT_EQ(Stringify::Type::String, Stringify::GetType(dumped));
}

TEST(StringifyTests, type_safety)
{
	bool raised = false;
	auto dumped = Stringify::Dump(1);

	try {
		auto failed = static_cast<std::string>(Stringify::Restore(dumped));
	} catch (const vist::Exception<ErrCode>& e) {
		raised = true;
		EXPECT_EQ(e.get(), ErrCode::TypeUnsafed);
	}

	EXPECT_TRUE(raised);
}

TEST(StringifyTests, restore_failed)
{
	bool raised = false;

	try {
		auto failed = Stringify::Restore("Not dumped message");
	} catch (const vist::Exception<ErrCode>& e) {
		raised = true;
		EXPECT_EQ(e.get(), ErrCode::LogicError);
	}

	EXPECT_TRUE(raised);
}
