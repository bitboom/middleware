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

#include <vist/client/query.hpp>

#include <chrono>
#include <thread>

using namespace vist;

class QueryTests : public testing::Test {};

TEST(QueryTests, query)
{
	auto rows = Query::Execute("SELECT * FROM policy");

	EXPECT_TRUE(rows.size() > 0);
}

TEST(QueryTests, admin_enrollment)
{
	/// Default policy admin is always exist.
	auto rows = Query::Execute("SELECT * FROM policy_admin");
	EXPECT_EQ(rows.size(), 1);

	rows = Query::Execute("INSERT INTO policy_admin (name) VALUES ('testAdmin')");
	EXPECT_EQ(rows.size(), 0);

	rows = Query::Execute("SELECT * FROM policy_admin");
	EXPECT_EQ(rows.size(), 2);

	Query::Execute("INSERT INTO policy_admin (name) VALUES ('testAdmin2')");
	rows = Query::Execute("SELECT * FROM policy_admin");
	EXPECT_EQ(rows.size(), 3);

	rows = Query::Execute("DELETE FROM policy_admin WHERE name = 'testAdmin'");
	EXPECT_EQ(rows.size(), 0);

	rows = Query::Execute("SELECT * FROM policy_admin");
	EXPECT_EQ(rows.size(), 2);

	Query::Execute("DELETE FROM policy_admin WHERE name = 'testAdmin2'");
	rows = Query::Execute("SELECT * FROM policy_admin");
	EXPECT_EQ(rows.size(), 1);
}

TEST(QueryTests, admin_activation)
{
	/// Default policy admin is always exist.
	auto rows = Query::Execute("SELECT * FROM policy_admin");
	EXPECT_EQ(rows.size(), 1);

	Query::Execute("INSERT INTO policy_admin (name) VALUES ('testAdmin')");
	rows = Query::Execute("SELECT * FROM policy_admin WHERE name = 'testAdmin'");
	EXPECT_EQ(rows.size(), 1);
	EXPECT_EQ(rows[0]["name"], "testAdmin");
	EXPECT_EQ(rows[0]["activated"], "0");

	Query::Execute("UPDATE policy_admin SET activated = 1 where name = 'testAdmin'");
	rows = Query::Execute("SELECT * FROM policy_admin WHERE name = 'testAdmin'");
	EXPECT_EQ(rows.size(), 1);
	EXPECT_EQ(rows[0]["name"], "testAdmin");
	EXPECT_EQ(rows[0]["activated"], "1");

	rows = Query::Execute("DELETE FROM policy_admin WHERE name = 'testAdmin'");
	EXPECT_EQ(rows.size(), 0);
}
