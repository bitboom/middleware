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

#include <vist/service/vist.hpp>
#include <vist/policy/api.hpp>

#include <iostream>
#include <chrono>
#include <thread>

using namespace vist;

class CoreTests : public testing::Test {};

TEST_F(CoreTests, query_select)
{
	auto rows = Vist::Query("SELECT * FROM policy");

	EXPECT_TRUE(rows.size() > 0);

	std::string statement = "SELECT * FROM policy WHERE name = 'bluetooth'";
	rows = Vist::Query(statement);

	EXPECT_EQ(rows.size(), 1);
	EXPECT_EQ(rows[0]["name"], "bluetooth");
}

TEST_F(CoreTests, query_update)
{
	policy::API::Admin::Enroll("admin");

	std::string statement = "SELECT * FROM policy WHERE name = 'bluetooth'";
	auto rows = Vist::Query(statement);
	/// Initial policy value
	EXPECT_EQ(rows[0]["value"], "I/1");

	statement = "UPDATE policy SET value = 'I/3' WHERE name = 'bluetooth'";
	rows = Vist::Query(statement);
	EXPECT_EQ(rows.size(), 0);

	statement = "SELECT * FROM policy WHERE name = 'bluetooth'";
	rows = Vist::Query(statement);
	EXPECT_EQ(rows[0]["value"], "I/3");

	policy::API::Admin::Disenroll("admin");
}
