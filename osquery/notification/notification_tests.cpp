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

#include <osquery/notification.h>
#include <osquery/logger.h>

using namespace osquery;

class NotificationTests : public testing::Test {};

TEST_F(NotificationTests, test_add_positive) {
	auto& notifier = Notification::instance();

	auto callback = [](const Row& row) {
		VLOG(1) << "NotifyCallback called:";
		for (const auto& r : row)
			VLOG(1) << "\t" << r.first << " : " << r.second;
	};

	auto s = notifier.add("test", std::move(callback));
	EXPECT_TRUE(s.ok());
}

TEST_F(NotificationTests, test_add_negative) {
	auto& notifier = Notification::instance();

	auto callback = [](const Row& row) {
		VLOG(1) << "NotifyCallback called:";
		for (const auto& r : row)
			VLOG(1) << "\t" << r.first << " : " << r.second;
	};

	auto s = notifier.add("", std::move(callback));
	EXPECT_FALSE(s.ok());
}

TEST_F(NotificationTests, test_emit_positive) {
	auto& notifier = Notification::instance();

	int called = 0;
	auto callback = [&](const Row& row) {
		VLOG(1) << "NotifyCallback called:";
		for (const auto& r : row)
			VLOG(1) << "\t" << r.first << " : " << r.second;
		called++;
	};

	auto s = notifier.add("test2", std::move(callback));
	EXPECT_TRUE(s.ok());

	Row row;
	row["foo"] = "bar";
	s = notifier.emit("test2", row);

	EXPECT_TRUE(s.ok());
	EXPECT_EQ(called, 1);
}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
