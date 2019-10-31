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

#include "../policy-manager.h"

namespace vist {
namespace policy {

class PolicyCoreTests : public testing::Test {};

TEST_F(PolicyCoreTests, policy_loader) {
	auto& manager = PolicyManager::Instance();

	EXPECT_TRUE(manager.providers.size() > 0);
	EXPECT_TRUE(manager.policies.size() > 0);
}

TEST_F(PolicyCoreTests, policy_set_get) {
	auto& manager = PolicyManager::Instance();
	manager.enroll("testAdmin");
	manager.set("bluetooth", PolicyValue(5), "testAdmin");

	auto policy = manager.get("bluetooth");
	EXPECT_EQ(policy.value, 5);

	manager.enroll("testAdmin1");
	manager.set("bluetooth", PolicyValue(10), "testAdmin1");

	/// Manager should return the strongest policy.
	policy = manager.get("bluetooth");
	EXPECT_EQ(policy.value, 5);

	manager.disenroll("testAdmin");
	manager.disenroll("testAdmin1");
}

} // namespace policy
} // namespace vist
