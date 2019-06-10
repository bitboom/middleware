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

#include <osquery/manager.h>
#include <osquery/logger.h>

using namespace osquery;

class ManagerTests : public testing::Test {};

TEST_F(ManagerTests, test_manager_load) {
	auto manager = OsqueryManager::Load();
	EXPECT_TRUE(manager != nullptr);
}

TEST_F(ManagerTests, test_manager_execute) {
	auto manager = OsqueryManager::Load();
	
	std::string query = "SELECT * FROM time";
	auto rows = manager->execute(query);
	EXPECT_EQ(rows.size(), 1);

	VLOG(1) << "[Test] time table rows:";
	VLOG(1) << "\t hour: " << rows[0]["hour"];
	VLOG(1) << "\t minutes: " << rows[0]["minutes"];
	VLOG(1) << "\t seconds: " << rows[0]["seconds"];
}

TEST_F(ManagerTests, test_manager_tables) {
	auto manager = OsqueryManager::Load();
	auto tables = manager->tables();
	EXPECT_TRUE(tables.size() > 0);

	VLOG(1) << "[Test] Enabled tables:";
	for (const auto& t : tables)
		VLOG(1) << "\t" << t;
}

TEST_F(ManagerTests, test_manager_columns) {
	auto manager = OsqueryManager::Load();
	auto columns = manager->columns("time");
	EXPECT_TRUE(columns.size() > 0);

	VLOG(1) << "[Test] Enabled columns of time table:";
	for (const auto& c : columns)
		VLOG(1) << "\t" << c;
}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
