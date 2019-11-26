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

#include <vist/policy/policy-manager.hpp>

namespace vist {
namespace policy {

TEST(PolicyCoreTests, policy_loader) {
	auto& manager = PolicyManager::Instance();

	EXPECT_TRUE(manager.providers.size() > 0);
	EXPECT_TRUE(manager.policies.size() > 0);
}

TEST(PolicyCoreTests, policy_set_get) {
	auto& manager = PolicyManager::Instance();
	manager.enroll("testAdmin");
	manager.set("bluetooth", PolicyValue(5), "testAdmin");

	auto policy = manager.get("bluetooth");
	EXPECT_EQ(static_cast<int>(policy), 5);

	manager.enroll("testAdmin1");
	manager.set("bluetooth", PolicyValue(10), "testAdmin1");

	/// Manager should return the strongest policy.
	policy = manager.get("bluetooth");
	EXPECT_EQ(static_cast<int>(policy), 10);

	manager.disenroll("testAdmin");
	manager.disenroll("testAdmin1");
}

TEST(PolicyCoreTests, policy_get_all) {
	auto& manager = PolicyManager::Instance();
	auto policies = manager.getAll();
	EXPECT_TRUE(policies.size() > 0);
}

TEST(PolicyCoreTests, policy_get_policy) {
	auto& manager = PolicyManager::Instance();
	const auto& policy = manager.getPolicy("bluetooth");
	EXPECT_EQ(policy->getName(), "bluetooth");

	bool raised = false;
	try {
		manager.getPolicy("fakePolicy");
	} catch (const vist::Exception<ErrCode>&) {
		raised = true;
	}
	EXPECT_TRUE(raised);
}

} // namespace policy
} // namespace vist
