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

#include "bluetooth.hpp"
#include "../test-util.hpp"

#include <vist/client/query.hpp>
#include <vist/client/schema/bluetooth.hpp>
#include <vist/exception.hpp>

#include <iostream>

#include <gtest/gtest.h>

using namespace vist;

TEST(BluetoothTests, change_policy_state)
{
	using namespace vist::policy::plugin;

	try {
		test::change_policy_state<BluetoothState>();
		test::change_policy_state<DesktopConnectivity>();
		test::change_policy_state<Pairing>();
		test::change_policy_state<Tethering>();
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
	Query::Execute("INSERT INTO policy_admin (name) VALUES ('vist-plugin-bluetooth-test')");

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

	Query::Execute("DELETE FROM policy_admin WHERE name = 'vist-plugin-bluetooth-test'");
}

TEST(BluetoothTest, query_builder)
{
	using namespace vist::schema;

	std::string query = BluetoothTable.selectAll();
	EXPECT_EQ(query, "SELECT * FROM bluetooth");

	query = BluetoothTable.update(Bluetooth::DesktopConnectivity = 3, Bluetooth::State = 7);
	EXPECT_EQ(query, "UPDATE bluetooth SET desktopConnectivity = 3, state = 7");

	query = BluetoothTable.update(Bluetooth::Pairing = 2, Bluetooth::Tethering = 9);
	EXPECT_EQ(query, "UPDATE bluetooth SET pairing = 2, tethering = 9");
}
