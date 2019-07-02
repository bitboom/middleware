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

using namespace osquery;

class PropertyTests : public testing::Test {};

TEST_F(PropertyTests, get) {
	Time result = { -1, -1, -1 };

	Property<Time> time;
	result.hour = time.get(&Time::hour);
	result.minutes = time.get(&Time::minutes);
	result.seconds = time.get(&Time::seconds);

	/// Once query execution
	VLOG(1) << "[Test] time table:";
	VLOG(1) << "\t hour: " << result.hour;
	VLOG(1) << "\t minutes: " << result.minutes;
	VLOG(1) << "\t seconds: " << result.seconds;

	/// Each query execution
	VLOG(1) << "[Test] time table:";
	VLOG(1) << "\t hour: " << Property<Time>().get(&Time::hour);
	VLOG(1) << "\t minutes: " << Property<Time>().get(&Time::minutes);
	VLOG(1) << "\t seconds: " <<  Property<Time>().get(&Time::seconds);

	EXPECT_NE(result.hour, -1);
	EXPECT_NE(result.minutes, -1);
	EXPECT_NE(result.seconds, -1);
}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
