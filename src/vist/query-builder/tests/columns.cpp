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

#include <gtest/gtest.h>

#include <vist/query-builder/column-pack.hpp>
#include <vist/query-builder/column.hpp>

using namespace vist::tsqb;
using namespace vist::tsqb::internal;

namespace {

struct Sample {
	int int1;
	int int2;
	std::string str1;
	std::string str2;
	bool bool1;
	bool bool2;
};

ColumnPack columns { Column("int1", &Sample::int1),
					 Column("int2", &Sample::int2),
					 Column("str1", &Sample::str1),
					 Column("str2", &Sample::str2),
					 Column("bool1", &Sample::bool1),
					 Column("bool2", &Sample::bool2) };
}

TEST(ColumnTests, size)
{
	EXPECT_EQ(columns.size(), 6);
}

TEST(ColumnTests, get_name)
{
	EXPECT_EQ(columns.getName(&Sample::int1), "int1");
	EXPECT_EQ(columns.getName(&Sample::int2), "int2");
	EXPECT_EQ(columns.getName(&Sample::str1), "str1");
	EXPECT_EQ(columns.getName(&Sample::str2), "str2");
	EXPECT_EQ(columns.getName(&Sample::bool1), "bool1");
	EXPECT_EQ(columns.getName(&Sample::bool2), "bool2");
}

TEST(ColumnTests, get_names)
{
	EXPECT_EQ(columns.getNames().size(), 6);
}
