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

#include <osquery/logger.h>

#include "../wifi_policy.h"

class WifiPolicyTests : public testing::Test {};

using namespace osquery;

TEST_F(WifiPolicyTests, Wifi) {
	WifiPolicy policy;
	policy.setWifi(true);
	EXPECT_EQ(policy.getWifi(), true);

	policy.setWifi(false);
	EXPECT_EQ(policy.getWifi(), false);
}

TEST_F(WifiPolicyTests, Profile) {
	WifiPolicy policy;
	policy.setProfile(true);
	EXPECT_EQ(policy.getProfile(), true);

	policy.setProfile(false);
	EXPECT_EQ(policy.getProfile(), false);
}

TEST_F(WifiPolicyTests, Hotspot) {
	WifiPolicy policy;
	policy.setHotspot(true);
	EXPECT_EQ(policy.getHotspot(), true);

	policy.setHotspot(false);
	EXPECT_EQ(policy.getHotspot(), false);
}
