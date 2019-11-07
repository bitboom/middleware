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

#include <vist/policy/sdk/policy-model.hpp>
#include <vist/policy/sdk/policy-provider.hpp>

#include <exception>

namespace {
	int g_value = -1;
} // anonymous namespace

using namespace vist::policy;

class PolicySDKTests : public testing::Test {};

class TestPolicyModel : public PolicyModel {
public:
	TestPolicyModel() : PolicyModel("test_policy", PolicyValue(1)) {}

	virtual void onChanged(const PolicyValue& value)
	{
		g_value = value;
	}
};

class TestPolicyModelFailed : public PolicyModel {
public:
	TestPolicyModelFailed() : PolicyModel("test_policy_failed", PolicyValue(1)) {}

	virtual void onChanged(const PolicyValue& value)
	{
		throw std::runtime_error("Intended exception for test.");
	}
};

TEST_F(PolicySDKTests, policy_model)
{
	TestPolicyModel policy;

	EXPECT_EQ(policy.getName(), "test_policy");
	EXPECT_EQ(policy.getInitial(), 1);

	// Policy value should be set once before use
	bool isRaised = false;
	try {
		auto value = policy.get();
	} catch (const std::exception&) {
		isRaised = true;
	}

	EXPECT_TRUE(isRaised);

	policy.set(PolicyValue(3));
	EXPECT_EQ(3, g_value);
	EXPECT_EQ(3, policy.get());
}

TEST_F(PolicySDKTests, policy_model_failed)
{
	TestPolicyModelFailed policy;

	EXPECT_EQ(policy.getName(), "test_policy_failed");
	EXPECT_EQ(policy.getInitial(), 1);

	bool isRaised = true;
	try {
		policy.set(PolicyValue(3));
	} catch (const std::exception&) {
		isRaised = true;
	}

	EXPECT_TRUE(isRaised);
}

TEST_F(PolicySDKTests, policy_provider)
{
	PolicyProvider provider("testProvider");
	provider.add(std::make_shared<TestPolicyModel>());
	provider.add(std::make_shared<TestPolicyModelFailed>());

	EXPECT_EQ(2, provider.size());
}
