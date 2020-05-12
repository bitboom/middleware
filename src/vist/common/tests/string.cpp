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

#include <gtest/gtest.h>

#include <vist/string.hpp>

using namespace vist;

TEST(StringTests, ltrim)
{
	std::string str = " a b c ";
	ltrim(str);
	EXPECT_EQ(str, "a b c ");
}

TEST(StringTests, rtrim)
{
	std::string str = " a b c ";
	rtrim(str);
	EXPECT_EQ(str, " a b c");
}

TEST(StringTests, trim)
{
	std::string str = " a b c ";
	trim(str);
	EXPECT_EQ(str, "a b c");
}

TEST(StringTests, split)
{
	std::string origin = "a b c";
	auto token = split(origin, std::regex("\\s"));
	if (token.size() == 3) {
		EXPECT_EQ(token[0], "a");
		EXPECT_EQ(token[1], "b");
		EXPECT_EQ(token[2], "c");
	} else {
		EXPECT_TRUE(false);
	}

	origin = "a,b,c";
	token = split(origin, std::regex(","));
	EXPECT_EQ(token.size(), 3);
	if (token.size() == 3) {
		EXPECT_EQ(token[0], "a");
		EXPECT_EQ(token[1], "b");
		EXPECT_EQ(token[2], "c");
	} else {
		EXPECT_TRUE(false);
	}
}
