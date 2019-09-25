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

using namespace policyd;

class PolicyCoreTests : public testing::Test {};

TEST_F(PolicyCoreTests, policy_loader) {
	PolicyManager pm;
	auto result = pm.loadProviders(PLUGIN_INSTALL_DIR);

	EXPECT_TRUE(result.first > 0);
	EXPECT_TRUE(result.second == 0);

	auto size = pm.loadPolicies();
	EXPECT_TRUE(size > 0);
}
