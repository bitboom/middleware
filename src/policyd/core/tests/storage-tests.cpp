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

#include <memory>

#include "../policy-storage.h"

using namespace policyd;

class PolicyStorageTests : public testing::Test {
public:
	void SetUp() override {
		/// TODO(Sangwan KWon): Change to test db
		this->storage = std::make_shared<PolicyStorage>(DB_PATH);
	}

	std::shared_ptr<PolicyStorage> getStorage() {
		return this->storage;
	}

private:
	std::shared_ptr<PolicyStorage> storage = nullptr;
};

TEST_F(PolicyStorageTests, initialize) {
	bool isRaised = false;

	try {
		PolicyStorage storage("/tmp/dummy");
	} catch (const std::exception&) {
		isRaised = true;
	}

	EXPECT_TRUE(isRaised);
}

TEST_F(PolicyStorageTests, enrollment) {
	auto storage = getStorage();
	EXPECT_FALSE(storage->isActivated());

	storage->enroll("testAdmin", 0);
	storage->enroll("testAdmin", 1);
	EXPECT_TRUE(storage->isActivated());

	storage->disenroll("testAdmin", 0);
	EXPECT_TRUE(storage->isActivated());

	storage->disenroll("testAdmin", 1);
	EXPECT_FALSE(storage->isActivated());
}

TEST_F(PolicyStorageTests, update) {
	auto storage = getStorage();
	storage->enroll("testAdmin", 0);

	bool isRaised = false;
	try {
		storage->update("testAdmin", 1, "bluetooth", PolicyValue(0));
	} catch (const std::exception&) {
		isRaised = true;
	}

	isRaised = false;
	try {
		storage->update("testAdmin", 0, "bluetooth", PolicyValue(0));
	} catch (const std::exception&) {
		isRaised = true;
	}
	EXPECT_FALSE(isRaised);

	isRaised = false;
	try {
		storage->update("testAdmin", 0, "FakePolicy", PolicyValue(0));
	} catch (const std::exception&) {
		isRaised = true;
	}
	EXPECT_TRUE(isRaised);

	storage->disenroll("testAdmin", 0);
}

TEST_F(PolicyStorageTests, strictest) {
	auto storage = getStorage();
	storage->enroll("testAdmin", 0);
	storage->enroll("testAdmin", 1);

	storage->update("testAdmin", 0, "bluetooth", PolicyValue(3));
	storage->update("testAdmin", 1, "bluetooth", PolicyValue(6));

	bool isRaised = false;
	try {
		auto value = storage->strictest("FakePolicy", 3);
	} catch (const std::exception&) {
		isRaised = true;
	}
	EXPECT_TRUE(isRaised);

	/// as global policy
	auto policy = storage->strictest("bluetooth");
	EXPECT_EQ(policy.value, 3);

	/// as domain policy
	policy = storage->strictest("bluetooth", 1);
	EXPECT_EQ(policy.value, 6);

	storage->disenroll("testAdmin", 0);
	storage->disenroll("testAdmin", 1);
}

TEST_F(PolicyStorageTests, strictest_all) {
	auto storage = getStorage();
	storage->enroll("testAdmin", 1);

	/// as global policy
	auto policies = storage->strictest();
	EXPECT_TRUE(policies.size() > 0);

	/// as domain policy
	policies = storage->strictest(1);
	EXPECT_TRUE(policies.size() > 0);

	storage->disenroll("testAdmin", 1);
}
