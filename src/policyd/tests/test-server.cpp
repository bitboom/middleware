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

#include "server/server.h"

#include <klay/testbench.h>

#include <iostream>

TESTCASE(LOAD_PLUGINS)
{
	DevicePolicyManager manager;
	auto result = manager.loadPolicyPlugins();

	// Temporary debug method
	auto providers = manager.getProviderList();
	for (const auto& provider : providers) {
		for (const auto& policy : provider->policies) {
			std::cout << "Policy Lists:" << std::endl;
			std::cout << "\t" << policy->name << std::endl;
		}
	}

	TEST_EXPECT(true, result.first > 0);
	TEST_EXPECT(true, result.second == 0);
}
