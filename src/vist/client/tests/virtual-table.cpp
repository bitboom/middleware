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

#include <vist/client/virtual-table.hpp>

#include <vist/client/schema/policy.hpp>
#include <vist/client/schema/time.hpp>

#include <vist/logger.hpp>

using namespace vist;
using namespace vist::schema;

class VirtualTableTests : public testing::Test {};

TEST(VirtualTableTests, time_row_at)
{
	Time result = { -1, -1, -1 };

	VirtualRow<Time> time;
	result.hour = time.at(&Time::hour);
	result.minutes = time.at(&Time::minutes);
	result.seconds = time.at(&Time::seconds);

	/// Once query execution
	INFO(VIST_CLIENT) << "[Test] time table:";
	INFO(VIST_CLIENT) << "\t hour: " << result.hour;
	INFO(VIST_CLIENT) << "\t minutes: " << result.minutes;
	INFO(VIST_CLIENT) << "\t seconds: " << result.seconds;

	/// Each query execution
	INFO(VIST_CLIENT) << "[Test] time table:";
	INFO(VIST_CLIENT) << "\t hour: " << VirtualRow<Time>().at(&Time::hour);
	INFO(VIST_CLIENT) << "\t minutes: " << VirtualRow<Time>().at(&Time::minutes);
	INFO(VIST_CLIENT) << "\t seconds: " <<  VirtualRow<Time>().at(&Time::seconds);

	EXPECT_NE(result.hour, -1);
	EXPECT_NE(result.minutes, -1);
	EXPECT_NE(result.seconds, -1);
}

TEST(VirtualTableTests, time_row_arry_op)
{
	Time result = { -1, -1, -1 };

	VirtualRow<Time> time;
	result.hour = time[&Time::hour];
	result.minutes = time[&Time::minutes];
	result.seconds = time[&Time::seconds];

	/// Once query execution
	INFO(VIST_CLIENT) << "[Test] time table:";
	INFO(VIST_CLIENT) << "\t hour: " << result.hour;
	INFO(VIST_CLIENT) << "\t minutes: " << result.minutes;
	INFO(VIST_CLIENT) << "\t seconds: " << result.seconds;

	EXPECT_NE(result.hour, -1);
	EXPECT_NE(result.minutes, -1);
	EXPECT_NE(result.seconds, -1);
}

TEST(VirtualTableTests, policy_int_table)
{
	VirtualTable<Policy<int>> table;
	EXPECT_TRUE(table.size() > 0);

	for(const auto& row : table) {
		Policy<int> policy = { row[&Policy<int>::name], row[&Policy<int>::value] };

		INFO(VIST_CLIENT) << "[Test] Policy<int> table:";
		INFO(VIST_CLIENT) << "\t name: " << policy.name;
		INFO(VIST_CLIENT) << "\t value: " << policy.value;
	}
}

TEST(VirtualTableTests, policy_int_filter)
{
	VirtualTable<Policy<int>> table;
	EXPECT_TRUE(table.size() > 0);

	auto row = table.filter(&Policy<int>::name, "sample-int-policy");
	auto value = row[&Policy<int>::value];
	EXPECT_TRUE(value > 0);
	EXPECT_EQ(row[&Policy<int>::name], "sample-int-policy");
}

TEST(VirtualTableTests, policy_str_table)
{
	VirtualTable<Policy<std::string>> table;
	EXPECT_TRUE(table.size() > 0);

	for(const auto& row : table) {
		Policy<std::string> policy = {
			row[&Policy<std::string>::name],
			row[&Policy<std::string>::value]
		};

		INFO(VIST_CLIENT) << "[Test] Policy<std::string> table:";
		INFO(VIST_CLIENT) << "\t name: " << policy.name;
		INFO(VIST_CLIENT) << "\t value: " << policy.value;
	}
}

TEST(VirtualTableTests, policy_str_filter)
{
	VirtualTable<Policy<std::string>> table;
	EXPECT_TRUE(table.size() > 0);

	auto row = table.filter(&Policy<std::string>::name, "sample-str-policy");
	auto value = row[&Policy<std::string>::value];
	EXPECT_TRUE(!value.empty());
	EXPECT_EQ(row[&Policy<std::string>::name], "sample-str-policy");
}
