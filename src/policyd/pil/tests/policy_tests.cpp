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

#include "../global-policy.h"
#include "../domain-policy.h"
#include "../policy-storage.h"
#include "../policy-event.h"

/// TODO(Sangwan): Refactor policy-model
/// => Let client know only global-policy or domain-policy

class PolicyModelTests : public testing::Test {};

class TestGlobalPolicy : public GlobalPolicy {
public:
	TestGlobalPolicy() : GlobalPolicy("test_policy")
	{
		PolicyEventNotifier::create("test_policy");
	}

	bool apply(const DataSetInt&, uid_t)
	{
		PolicyEventNotifier::emit("test_policy", "allowed");
		return true;
	}
};

TEST_F(PolicyModelTests, global_policy) {
	AbstractPolicyProvider provider;
	provider.policies.emplace_back(std::make_shared<TestGlobalPolicy>());

	EXPECT_EQ(provider.policies.size(), 1);
}
