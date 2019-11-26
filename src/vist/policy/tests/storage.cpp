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

#include <vist/policy/policy-storage.hpp>

using namespace vist::policy;

class PolicyStorageTests : public testing::Test {
public:
	void SetUp() override
	{
		this->storage = std::make_shared<PolicyStorage>(DB_PATH);
	}

	std::shared_ptr<PolicyStorage> getStorage()
	{
		return this->storage;
	}

private:
	std::shared_ptr<PolicyStorage> storage = nullptr;
};

TEST_F(PolicyStorageTests, initialize)
{
	bool isRaised = false;

	try {
		// DB is maden at run-time
		PolicyStorage storage("/tmp/dummy");
	} catch (const std::exception& e) {
		isRaised = true;
	}

	EXPECT_FALSE(isRaised);
}

TEST_F(PolicyStorageTests, enrollment)
{
	auto storage = getStorage();
	/// Since default admin exists, storage is always activated.
	EXPECT_TRUE(storage->isActivated());

	storage->enroll("testAdmin0");
	storage->enroll("testAdmin1");
	EXPECT_TRUE(storage->isActivated());

	storage->disenroll("testAdmin0");
	EXPECT_TRUE(storage->isActivated());

	storage->disenroll("testAdmin1");
	EXPECT_TRUE(storage->isActivated());
}

TEST_F(PolicyStorageTests, update)
{
	auto storage = getStorage();
	storage->enroll("testAdmin");

	bool isRaised = false;
	try {
		storage->update("fakeAdmin", "bluetooth", PolicyValue(0));
	} catch (const std::exception&) {
		isRaised = true;
	}

	isRaised = false;
	try {
		storage->update("testAdmin", "bluetooth", PolicyValue(0));
	} catch (const std::exception&) {
		isRaised = true;
	}
	EXPECT_FALSE(isRaised);

	isRaised = false;
	try {
		storage->update("testAdmin", "FakePolicy", PolicyValue(0));
	} catch (const std::exception&) {
		isRaised = true;
	}
	EXPECT_TRUE(isRaised);

	storage->disenroll("testAdmin");
}

TEST_F(PolicyStorageTests, admin_list)
{
	auto storage = getStorage();

	auto admins = storage->getAdmins();
	EXPECT_EQ(admins.size(), 1);

	storage->enroll("testAdmin1");
	admins = storage->getAdmins();
	EXPECT_EQ(admins.size(), 2);

	storage->enroll("testAdmin2");
	admins = storage->getAdmins();
	EXPECT_EQ(admins.size(), 3);

	storage->disenroll("testAdmin2");
	admins = storage->getAdmins();
	EXPECT_EQ(admins.size(), 2);

	storage->disenroll("testAdmin1");
	admins = storage->getAdmins();
	EXPECT_EQ(admins.size(), 1);
}

TEST_F(PolicyStorageTests, default_admin)
{
	auto storage = getStorage();
	bool isRaised = false;

	/// Cannot disenroll default admin
	try {
		storage->disenroll(DEFAULT_ADMIN_PATH);
	} catch (const std::exception& e) {
		isRaised = true;
	}

	EXPECT_TRUE(isRaised);
}
