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

#include "schema.hpp"

using namespace vist::tsqb;
using namespace vist::test;

TEST(QueryBuilderExpressionTests, equal)
{
	std::string dump = (Table1::Column1 == 3);
	EXPECT_EQ(dump, "=");

	dump = (Table1::Column2 == "string");
	EXPECT_EQ(dump, "=");
}

TEST(QueryBuilderExpressionTests, greater)
{
	std::string dump = (Table1::Column1 > 3);
	EXPECT_EQ(dump, ">");
}

TEST(QueryBuilderExpressionTests, lesser)
{
	std::string dump = (Table1::Column1 < 3);
	EXPECT_EQ(dump, "<");
}

TEST(QueryBuilderExpressionTests, and_)
{
	std::string dump = (Table1::Column1 < 3) && (Table1::Column1 == 3);
	EXPECT_EQ(dump, "AND");
}

TEST(QueryBuilderExpressionTests, or_)
{
	std::string dump = (Table1::Column1 < 3) || (Table1::Column2 == "text");
	EXPECT_EQ(dump, "OR");
}
