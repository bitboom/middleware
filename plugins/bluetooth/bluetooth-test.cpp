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

#include "bluetooth.hpp"
#include "../test-util.hpp"

#include <vist/exception.hpp>

#include <gtest/gtest.h>

using namespace vist;
using namespace vist::policy::plugin;

TEST(BluetoothTests, change_policy_state)
{
	try {
		test::change_policy_state<BluetoothState>();
		test::change_policy_state<DesktopConnectivity>();
		test::change_policy_state<Pairing>();
		test::change_policy_state<Tethering>();
	} catch(const vist::Exception<ErrCode>& e) {
		EXPECT_TRUE(false) << e.what();
	}
}
