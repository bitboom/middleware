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

#include <vist/sdk/policy-model.hpp>
#include <vist/sdk/policy-provider.hpp>

#include <exception>

namespace {
	int g_value = -1;
} // anonymous namespace

using namespace vist;
using namespace vist::policy;

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

	virtual void onChanged(const PolicyValue&)
	{
		throw std::runtime_error("Intended exception for test.");
	}
};

TEST(PolicySDKTests, policy_value_int)
{
	PolicyValue value(1);
	EXPECT_EQ(Stringify::Type::Integer, value.getType());
	EXPECT_EQ(static_cast<int>(value), 1);
}

TEST(PolicySDKTests, policy_value_string)
{
	PolicyValue value("TEXT");
	EXPECT_EQ(Stringify::Type::String, value.getType());
	EXPECT_EQ(static_cast<std::string>(value), "TEXT");
}

TEST(PolicySDKTests, policy_value_dump)
{
	PolicyValue intValue(1);
	EXPECT_EQ("I/1", intValue.dump());

	PolicyValue strValue("TEXT");
	EXPECT_EQ("S/TEXT", strValue.dump());
}

TEST(PolicySDKTests, policy_model)
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

TEST(PolicySDKTests, policy_model_failed)
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

TEST(PolicySDKTests, policy_provider)
{
	PolicyProvider provider("testProvider");
	provider.add(std::make_shared<TestPolicyModel>());
	provider.add(std::make_shared<TestPolicyModelFailed>());

	EXPECT_EQ(2, provider.size());
}
