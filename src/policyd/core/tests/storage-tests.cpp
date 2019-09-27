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

#include "../policy-storage.h"

using namespace policyd;

class PolicyStorageTests : public testing::Test {};

TEST_F(PolicyStorageTests, initialize) {
	bool isRaised = false;

	try {
		/// TODO(Sangwan KWon): Change to test db
		PolicyStorage storage(DB_PATH);
	} catch (const std::exception&) {
		isRaised = true;
	}

	EXPECT_FALSE(isRaised);

	isRaised = false;
	try {
		PolicyStorage storage("/tmp/dummy");
	} catch (const std::exception&) {
		isRaised = true;
	}

	EXPECT_TRUE(isRaised);
}

TEST_F(PolicyStorageTests, enrollment) {
	PolicyStorage storage(DB_PATH);
	EXPECT_FALSE(storage.isActivated());

	storage.enroll("testAdmin", 0);
	storage.enroll("testAdmin", 1);
	EXPECT_TRUE(storage.isActivated());

	storage.disenroll("testAdmin", 0);
	EXPECT_TRUE(storage.isActivated());

	storage.disenroll("testAdmin", 1);
	EXPECT_FALSE(storage.isActivated());
}
