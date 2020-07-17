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

#include <vist/service/vistd.hpp>
#include <vist/policy/api.hpp>

#include <iostream>
#include <chrono>

using namespace vist;

class CoreTests : public testing::Test {};

TEST_F(CoreTests, query_select)
{
	auto rows = Vistd::Query("SELECT * FROM policy");

	EXPECT_TRUE(rows.size() > 0);

	std::string statement = "SELECT * FROM policy WHERE name = 'sample_int_policy'";
	rows = Vistd::Query(statement);

	if (rows.size() == 1)
		EXPECT_EQ(rows[0]["name"], "sample_int_policy");
	else
		EXPECT_TRUE(false);
}

TEST_F(CoreTests, query_update)
{
	policy::API::Admin::Enroll("vist-test");

	std::string statement = "SELECT * FROM policy WHERE name = 'sample_int_policy'";
	auto rows = Vistd::Query(statement);
	/// Initial policy value
	EXPECT_EQ(rows[0]["value"], "I/7");

	statement = "UPDATE policy SET value = 'I/10' WHERE name = 'sample_int_policy'";
	rows = Vistd::Query(statement);
	EXPECT_EQ(rows.size(), 0);

	statement = "SELECT * FROM policy WHERE name = 'sample_int_policy'";
	rows = Vistd::Query(statement);
	EXPECT_EQ(rows[0]["value"], "I/10");

	policy::API::Admin::Disenroll("vist-test");
}
