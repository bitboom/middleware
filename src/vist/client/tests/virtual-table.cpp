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
#include <vist/client/schema/processes.hpp>
#include <vist/client/schema/time.hpp>

#include <vist/logger.hpp>

using namespace vist;
using namespace vist::schema;

class VirtualTableTests : public testing::Test {};

TEST(VirtualTableTests, time_row_at) {
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

TEST(VirtualTableTests, time_row_arry_op) {
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

TEST(VirtualTableTests, processes_table) {
	Processes result;
	VirtualTable<Processes> processes;
	EXPECT_TRUE(processes.size() > 0);

	for(auto& p : processes) {
		EXPECT_TRUE(p.size() > 0);
		result.pid = p.at(&Processes::pid);
		result.name = p.at(&Processes::name);
		result.path = p.at(&Processes::path);
		result.cmdline = p.at(&Processes::cmdline);
		result.uid = p.at(&Processes::uid);
		result.gid = p.at(&Processes::gid);
		result.euid = p.at(&Processes::euid);
		result.egid = p.at(&Processes::egid);
		result.on_disk = p.at(&Processes::on_disk);
		result.parent = p.at(&Processes::parent);

		INFO(VIST_CLIENT) << "[Test] Processes table:";
		INFO(VIST_CLIENT) << "\t pid: " << result.pid;
		INFO(VIST_CLIENT) << "\t name: " << result.name;
		INFO(VIST_CLIENT) << "\t path: " << result.path;
		INFO(VIST_CLIENT) << "\t cmdline: " << result.cmdline;
		INFO(VIST_CLIENT) << "\t uid: " << result.uid;
		INFO(VIST_CLIENT) << "\t gid: " << result.gid;
		INFO(VIST_CLIENT) << "\t euid: " << result.euid;
		INFO(VIST_CLIENT) << "\t egid: " << result.egid;
		INFO(VIST_CLIENT) << "\t on_disk: " << result.on_disk;
		INFO(VIST_CLIENT) << "\t parent: " << result.parent;
	}
}

TEST(VirtualTableTests, policy_table) {
	VirtualTable<Policy> table;
	EXPECT_TRUE(table.size() > 0);

	for(const auto& row : table) {
		Policy policy = { row[&Policy::name], row[&Policy::value] };

		INFO(VIST_CLIENT) << "[Test] Policy table:";
		INFO(VIST_CLIENT) << "\t name: " << policy.name;
		INFO(VIST_CLIENT) << "\t value: " << policy.value;
	}
}
