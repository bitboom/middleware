/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/thread-pool.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace vist;

TEST(ThreadPoolTests, once)
{
	int count = 0;
	auto task = [&count]{ count++; };

	ThreadPool worker(5);
	worker.submit(task);

	std::this_thread::sleep_for(std::chrono::seconds(1));
	EXPECT_EQ(count, 1);
}

TEST(ThreadPoolTests, multiple)
{
	int count = 0;
	auto task = [&count]{ count++; };

	ThreadPool worker(5);
	std::size_t repeat = 10;
	while (repeat--)
		worker.submit(task);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	EXPECT_TRUE(count > 5);
}
