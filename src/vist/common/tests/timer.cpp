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

#include <vist/timer.hpp>

#include <string>

using namespace vist;

TEST(TimerTests, exec_once)
{
	int count = 0;

	auto task = [&count]() {
		count++;
	};

	auto predicate = []() -> bool {
		return true;
	};

	/// Check every 1 second until predicate() returns true and
	/// execute task() once when predicates() returns true.
	Timer::ExecOnce(task, predicate, 1);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	EXPECT_EQ(count, 1);
}
