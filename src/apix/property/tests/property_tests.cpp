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

#include <osquery/logger.h>

#include <property.h>

#include <schema/time.h>
#include <schema/processes.h>

using namespace osquery;

class PropertyTests : public testing::Test {};

TEST_F(PropertyTests, property) {
	Time result = { -1, -1, -1 };

	Property<Time> time;
	result.hour = time.at(&Time::hour);
	result.minutes = time.at(&Time::minutes);
	result.seconds = time.at(&Time::seconds);

	/// Once query execution
	LOG(INFO) << "[Test] time table:";
	LOG(INFO) << "\t hour: " << result.hour;
	LOG(INFO) << "\t minutes: " << result.minutes;
	LOG(INFO) << "\t seconds: " << result.seconds;

	/// Each query execution
	LOG(INFO) << "[Test] time table:";
	LOG(INFO) << "\t hour: " << Property<Time>().at(&Time::hour);
	LOG(INFO) << "\t minutes: " << Property<Time>().at(&Time::minutes);
	LOG(INFO) << "\t seconds: " <<  Property<Time>().at(&Time::seconds);

	EXPECT_NE(result.hour, -1);
	EXPECT_NE(result.minutes, -1);
	EXPECT_NE(result.seconds, -1);
}

TEST_F(PropertyTests, propertyArrayOp) {
	Time result = { -1, -1, -1 };

	Property<Time> time;
	result.hour = time[&Time::hour];
	result.minutes = time[&Time::minutes];
	result.seconds = time[&Time::seconds];

	/// Once query execution
	LOG(INFO) << "[Test] time table:";
	LOG(INFO) << "\t hour: " << result.hour;
	LOG(INFO) << "\t minutes: " << result.minutes;
	LOG(INFO) << "\t seconds: " << result.seconds;

	EXPECT_NE(result.hour, -1);
	EXPECT_NE(result.minutes, -1);
	EXPECT_NE(result.seconds, -1);
}

TEST_F(PropertyTests, propertiesProcesses) {
	Processes result;
	Properties<Processes> processes;
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

		LOG(INFO) << "[Test] Processes table:";
		LOG(INFO) << "\t pid: " << result.pid;
		LOG(INFO) << "\t name: " << result.name;
		LOG(INFO) << "\t path: " << result.path;
		LOG(INFO) << "\t cmdline: " << result.cmdline;
		LOG(INFO) << "\t uid: " << result.uid;
		LOG(INFO) << "\t gid: " << result.gid;
		LOG(INFO) << "\t euid: " << result.euid;
		LOG(INFO) << "\t egid: " << result.egid;
		LOG(INFO) << "\t on_disk: " << result.on_disk;
		LOG(INFO) << "\t parent: " << result.parent;
	}
}
