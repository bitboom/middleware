/*
 *  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/policy/policy-manager.hpp>
#include <vist/service/vistd.hpp>

namespace vist {
namespace policy {

namespace test{

	void init()
	{
		// Policies are loaded via dynamic table when actual daemon starts.
		static_cast<void>(Vistd::Instance());
	}

} // anonymous namespace

TEST(PolicyCoreTests, policy_set_get_int)
{
	test::init();

	auto& manager = PolicyManager::Instance();
	manager.enroll("testAdmin");
	manager.set("sample_int_policy", PolicyValue(5), "testAdmin");

	/// Default Admin's policy is more strict. (Because of initial value: 7)
	auto policy = manager.get("sample_int_policy");
	EXPECT_EQ(static_cast<int>(policy), 7);

	manager.enroll("testAdmin1");
	manager.set("sample_int_policy", PolicyValue(10), "testAdmin1");

	/// Manager should return the strongest policy.
	policy = manager.get("sample_int_policy");
	EXPECT_EQ(static_cast<int>(policy), 10);

	manager.disenroll("testAdmin");
	manager.disenroll("testAdmin1");
}

TEST(PolicyCoreTests, policy_set_get_str)
{
	test::init();

	auto& manager = PolicyManager::Instance();
	manager.enroll("testAdmin");
	manager.set("sample_str_policy", PolicyValue("AAA"), "testAdmin");

	auto policy = manager.get("sample_str_policy");
	EXPECT_EQ(static_cast<std::string>(policy), "AAA");

	manager.enroll("testAdmin1");
	manager.set("sample_str_policy", PolicyValue("BBB"), "testAdmin1");

	/// Manager should return the strongest policy.
	policy = manager.get("sample_str_policy");
	EXPECT_EQ(static_cast<std::string>(policy), "AAA");

	manager.disenroll("testAdmin");
	manager.disenroll("testAdmin1");
}

TEST(PolicyCoreTests, policy_get_all)
{
	test::init();

	auto& manager = PolicyManager::Instance();
	auto policies = manager.getAll();
	EXPECT_TRUE(policies.size() > 0);
}

TEST(PolicyCoreTests, policy_get_policy)
{
	test::init();

	auto& manager = PolicyManager::Instance();
	const auto& policy = manager.getPolicy("sample_int_policy");
	EXPECT_EQ(policy->getName(), "sample_int_policy");

	bool raised = false;
	try {
		manager.getPolicy("fakePolicy");
	} catch (const vist::Exception<ErrCode>&) {
		raised = true;
	}
	EXPECT_TRUE(raised);
}

TEST(PolicyCoreTests, admin)
{
	auto& manager = PolicyManager::Instance();
	manager.enroll("testAdmin");

	try {
		manager.activate("testAdmin", true);
		manager.activate("testAdmin", false);
	} catch (const vist::Exception<ErrCode>& e) {
		EXPECT_TRUE(false) << e.what();
	}

	bool raised = false;
	try {
		manager.activate("fakeAdmin", true);
	} catch (const vist::Exception<ErrCode>&) {
		raised = true;
	}
	EXPECT_TRUE(raised);

	manager.disenroll("testAdmin");
}

TEST(PolicyCoreTests, is_activated)
{
	auto& manager = PolicyManager::Instance();
	manager.enroll("testAdmin1");
	manager.enroll("testAdmin2");

	EXPECT_FALSE(manager.isActivated());

	manager.activate("testAdmin1", true);
	EXPECT_TRUE(manager.isActivated());

	manager.activate("testAdmin2", true);
	EXPECT_TRUE(manager.isActivated());

	manager.activate("testAdmin1", false);
	EXPECT_TRUE(manager.isActivated());

	manager.activate("testAdmin2", false);
	EXPECT_FALSE(manager.isActivated());

	manager.disenroll("testAdmin1");
	manager.disenroll("testAdmin2");
}

} // namespace policy
} // namespace vist
