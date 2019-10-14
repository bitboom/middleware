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

#include <osquery_manager.h>

#include <osquery/logger.h>

#include <policyd/core/policy-manager.h>

using namespace osquery;

class PolicydTests : public testing::Test {};

TEST_F(PolicydTests, select) {
	std::string query = "SELECT * FROM policy";
	auto rows = OsqueryManager::execute(query);
	EXPECT_TRUE(rows.size() > 0);

	LOG(INFO) << "[Test] policy table rows:";
	for (auto& r : rows) {
		LOG(INFO) << "\t name: " << r["name"];
		LOG(INFO) << "\t value: " << r["value"];
	}

	query = "SELECT * FROM policy WHERE name = 'bluetooth'";
	rows = OsqueryManager::execute(query);

	LOG(INFO) << "[Test] policy table rows with where clause:";
	for (auto& r : rows) {
		LOG(INFO) << "\t name: " << r["name"];
		LOG(INFO) << "\t value: " << r["value"];
	}

	EXPECT_EQ(rows.size(), 1);
}

TEST_F(PolicydTests, update) {
	auto& manager = policyd::PolicyManager::Instance();
	manager.enroll("admin", 0);

	std::string query = "SELECT * FROM policy WHERE name = 'bluetooth'";
	auto rows = OsqueryManager::execute(query);
	/// Initial policy value
	EXPECT_EQ(rows[0]["value"], std::to_string(1));

	query = "UPDATE policy SET value = '3' WHERE name = 'bluetooth'";
	rows = OsqueryManager::execute(query);
	EXPECT_EQ(rows.size(), 0);

	query = "SELECT * FROM policy WHERE name = 'bluetooth'";
	rows = OsqueryManager::execute(query);
	/// Initial policy value
	EXPECT_EQ(rows[0]["value"], std::to_string(3));

	manager.disenroll("admin", 0);
}
