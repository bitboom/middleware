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
	VLOG(1) << "[Test] time table:";
	VLOG(1) << "\t hour: " << result.hour;
	VLOG(1) << "\t minutes: " << result.minutes;
	VLOG(1) << "\t seconds: " << result.seconds;

	/// Each query execution
	VLOG(1) << "[Test] time table:";
	VLOG(1) << "\t hour: " << Property<Time>().at(&Time::hour);
	VLOG(1) << "\t minutes: " << Property<Time>().at(&Time::minutes);
	VLOG(1) << "\t seconds: " <<  Property<Time>().at(&Time::seconds);

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
	VLOG(1) << "[Test] time table:";
	VLOG(1) << "\t hour: " << result.hour;
	VLOG(1) << "\t minutes: " << result.minutes;
	VLOG(1) << "\t seconds: " << result.seconds;

	EXPECT_NE(result.hour, -1);
	EXPECT_NE(result.minutes, -1);
	EXPECT_NE(result.seconds, -1);
}

TEST_F(PropertyTests, properties) {
	Processes result = {
		-1, /// pid
		"", /// name
		"", /// path
		"", /// cmdline
		-1, /// uid
		-1, /// gid
		-1, /// euid
		-1, /// egid
		"", /// on_disk
//		"", /// wired_size
		"", /// resident_size
		"", /// phys_footprint
		"", /// user_time
		"", /// system_time
		"", /// start_time
		-1  /// parent
		};

	Properties<Processes> processes;

	for(auto& p : processes) {
		result.pid = p.at(&Processes::pid);
		result.name = p.at(&Processes::name);
		result.path = p.at(&Processes::path);
		result.cmdline = p.at(&Processes::cmdline);
		result.uid = p.at(&Processes::uid);
		result.gid = p.at(&Processes::gid);
		result.euid = p.at(&Processes::euid);
		result.egid = p.at(&Processes::egid);
		result.on_disk = p.at(&Processes::on_disk);
//		result.wired_size = p.at(&Processes::wired_size);
		result.resident_size = p.at(&Processes::resident_size);
		result.phys_footprint = p.at(&Processes::phys_footprint);
		result.user_time = p.at(&Processes::user_time);
		result.system_time = p.at(&Processes::system_time);
		result.start_time = p.at(&Processes::start_time);
		result.parent = p.at(&Processes::parent);

		VLOG(1) << "[Test] Processes table:";
		VLOG(1) << "\t pid: " << result.pid;
		VLOG(1) << "\t name: " << result.name;
		VLOG(1) << "\t path: " << result.path;
		VLOG(1) << "\t cmdline: " << result.cmdline;
		VLOG(1) << "\t uid: " << result.uid;
		VLOG(1) << "\t gid: " << result.gid;
		VLOG(1) << "\t euid: " << result.euid;
		VLOG(1) << "\t egid: " << result.egid;
		VLOG(1) << "\t on_disk: " << result.on_disk;
//		VLOG(1) << "\t wired_size: " << result.wired_size;
		VLOG(1) << "\t resident_size: " << result.resident_size;
		VLOG(1) << "\t phys_footprint: " << result.phys_footprint;
		VLOG(1) << "\t user_time: " << result.user_time;
		VLOG(1) << "\t system_time: " << result.system_time;
		VLOG(1) << "\t start_time: " << result.start_time;
		VLOG(1) << "\t parent: " << result.parent;
	}

//	EXPECT_NE(result.hour, -1);
}
