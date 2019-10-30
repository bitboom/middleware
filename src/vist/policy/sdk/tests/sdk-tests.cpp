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

#include "../domain-policy.h"
#include "../global-policy.h"
#include "../policy-provider.h"

#include <exception>

namespace {
	int g_value = -1;
	int d_value = -1;
	uid_t d_uid = 0;
} // anonymous namespace

using namespace vist::policy;

class PolicySDKTests : public testing::Test {};

class TestGlobalPolicy : public GlobalPolicy {
public:
	TestGlobalPolicy() : GlobalPolicy("test_policy", PolicyValue(1)) {}

	virtual void onChanged(const PolicyValue& value) {
		g_value = value;
	}
};

TEST_F(PolicySDKTests, global_policy) {
	TestGlobalPolicy policy;

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

class TestDomainPolicy : public DomainPolicy {
public:
	TestDomainPolicy() : DomainPolicy("test_policy", PolicyValue(1)) {}

	virtual void onChanged(uid_t domain, const PolicyValue& value) {
		d_uid = domain;
		d_value = value;
	}
};

TEST_F(PolicySDKTests, domain_policy) {
	TestDomainPolicy policy;
	uid_t domain = 5001;

	EXPECT_EQ(policy.getName(), "test_policy");
	EXPECT_EQ(policy.getInitial(), 1);

	// Policy value should be set once before use
	bool isRaised = false;
	try {
		auto value = policy.get(domain);
	} catch (const std::exception&) {
		isRaised = true;
	}

	EXPECT_TRUE(isRaised);

	policy.set(domain, PolicyValue(3));
	EXPECT_EQ(d_uid, domain);
	EXPECT_EQ(3, policy.get(domain));
}

TEST_F(PolicySDKTests, policy_provider) {
	PolicyProvider provider("testProvider");
	provider.add(std::make_shared<TestGlobalPolicy>());
	provider.add(std::make_shared<TestDomainPolicy>());

	EXPECT_EQ(1, provider.gsize());
	EXPECT_EQ(1, provider.dsize());
}
