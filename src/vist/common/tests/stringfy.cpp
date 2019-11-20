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

TEST(StringfyTests, integer_dump)
{
	EXPECT_EQ("I/-11", Stringfy::Dump(-11));
	EXPECT_EQ("I/-1", Stringfy::Dump(-1));
	EXPECT_EQ("I/0", Stringfy::Dump(0));
	EXPECT_EQ("I/1", Stringfy::Dump(1));
	EXPECT_EQ("I/11", Stringfy::Dump(11));
}

TEST(StringfyTests, string_dump)
{
	EXPECT_EQ("S/", Stringfy::Dump(""));
	EXPECT_EQ("S/TEXT", Stringfy::Dump("TEXT"));
}

TEST(StringfyTests, integer_restore)
{
	auto dumped = Stringfy::Dump(-1);
	EXPECT_EQ(-1, Stringfy::Restore(dumped));

	dumped = Stringfy::Dump(0);
	EXPECT_EQ(0, Stringfy::Restore(dumped));

	dumped = Stringfy::Dump(1);
	EXPECT_EQ(1, Stringfy::Restore(dumped));
}

TEST(StringfyTests, string_restore)
{
	auto dumped = Stringfy::Dump("");
	EXPECT_EQ(std::string(""), static_cast<std::string>(Stringfy::Restore(dumped)));

	dumped = Stringfy::Dump("TEXT");
	EXPECT_EQ(std::string("TEXT"), static_cast<std::string>(Stringfy::Restore(dumped)));
}

TEST(StringfyTests, get_type)
{
	auto dumped = Stringfy::Dump(0);
	EXPECT_EQ(Stringfy::Type::Integer, Stringfy::GetType(dumped));

	dumped = Stringfy::Dump("Text");
	EXPECT_EQ(Stringfy::Type::String, Stringfy::GetType(dumped));
}

TEST(StringfyTests, type_safety)
{
	bool raised = false;
	auto dumped = Stringfy::Dump(1);

	try {
		auto failed = static_cast<std::string>(Stringfy::Restore(dumped));
	} catch (const vist::Exception<ErrCode>& e) {
		raised = true;
		EXPECT_EQ(e.get(), ErrCode::TypeUnsafed);
	}

	EXPECT_TRUE(raised);
}

TEST(StringfyTests, restore_failed)
{
	bool raised = false;

	try {
		auto failed = Stringfy::Restore("Not dumped message");
	} catch (const vist::Exception<ErrCode>& e) {
		raised = true;
		EXPECT_EQ(e.get(), ErrCode::LogicError);
	}

	EXPECT_TRUE(raised);
}
