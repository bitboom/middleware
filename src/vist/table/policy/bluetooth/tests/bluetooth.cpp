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

#include "../policy.hpp"

#include <vist/client/query.hpp>
#include <vist/exception.hpp>
#include <vist/sdk/policy-model.hpp>
#include <vist/sdk/policy-value.hpp>

#include <iostream>

#include <gtest/gtest.h>

using namespace vist;
using namespace vist::policy;

namespace {

template <typename T>
void change_policy_state()
{
	std::shared_ptr<PolicyModel> policy = std::make_shared<T>();
	/// enable policy
	policy->onChanged(PolicyValue(1));
	/// disable policy
	policy->onChanged(PolicyValue(0));
}

} // anonymous namespace

TEST(BluetoothTests, change_policy_state)
{
	try {
		change_policy_state<BluetoothState>();
		change_policy_state<DesktopConnectivity>();
		change_policy_state<Pairing>();
		change_policy_state<Tethering>();
	} catch(const vist::Exception<ErrCode>& e) {
		EXPECT_TRUE(false) << e.what();
	}
}

TEST(BluetoothTests, get_policies)
{
	auto rows = Query::Execute("SELECT * FROM bluetooth");

	EXPECT_TRUE(rows.size() == 1);
}

TEST(BluetoothTests, set_policies)
{
	// Binany should be enrolled as admin to control policy.
	Query::Execute("INSERT INTO policy_admin (name) VALUES ('vist-bluetooth-policy-test')");

	Query::Execute("UPDATE bluetooth SET desktopConnectivity = 3, state = 7");
	Query::Execute("UPDATE bluetooth SET pairing = 2, tethering = 9");

	auto rows = Query::Execute("SELECT * FROM bluetooth");
	if (rows.size() == 1) {
		EXPECT_EQ(rows[0]["state"], "7");
		EXPECT_EQ(rows[0]["desktopConnectivity"], "3");
		EXPECT_EQ(rows[0]["pairing"], "2");
		EXPECT_EQ(rows[0]["tethering"], "9");
	} else {
		EXPECT_TRUE(false);
	}

	Query::Execute("DELETE FROM policy_admin WHERE name = 'vist-bluetooth-policy-test'");
}
