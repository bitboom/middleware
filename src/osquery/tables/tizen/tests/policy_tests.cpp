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

#include <osquery/sql.h>
#include <osquery/logger.h>

#include <dpm/device-policy-manager.h>
#include <dpm/pil/policy-client.h>

class PolicyTests : public testing::Test {};

using namespace osquery;

TEST_F(PolicyTests, Bluetooth) {
	std::shared_ptr<void> handle(dpm_manager_create(), dpm_manager_destroy);
	if (handle == nullptr)
		throw std::runtime_error("Cannot create dpm-client handle.");

	::Status<bool> status { true };

	DevicePolicyClient &client = GetDevicePolicyClient(handle.get());
	status = client.methodCall<bool>("Bluetooth::getModeChangeState");
	EXPECT_EQ(true, status.get());

	status = client.methodCall<bool>("Bluetooth::getDesktopConnectivityState");
	EXPECT_EQ(true, status.get());

	status = client.methodCall<bool>("Bluetooth::getTetheringState");
	EXPECT_EQ(true, status.get());

	status = client.methodCall<bool>("Bluetooth::getPairingState");
	EXPECT_EQ(true, status.get());
}

TEST_F(PolicyTests, Wifi) {
	std::shared_ptr<void> handle(dpm_manager_create(), dpm_manager_destroy);
	if (handle == nullptr)
		throw std::runtime_error("Cannot create dpm-client handle.");

	::Status<bool> status { true };

	DevicePolicyClient &client = GetDevicePolicyClient(handle.get());
	status = client.methodCall<bool>("Wifi::getState");
	EXPECT_EQ(true, status.get());

	status = client.methodCall<bool>("Wifi::isProfileChangeRestricted");
	EXPECT_EQ(true, status.get());

	status = client.methodCall<bool>("Wifi::getHotspotState");
	EXPECT_EQ(true, status.get());
}

TEST_F(PolicyTests, Usb) {
	std::shared_ptr<void> handle(dpm_manager_create(), dpm_manager_destroy);
	if (handle == nullptr)
		throw std::runtime_error("Cannot create dpm-client handle.");

	::Status<bool> status { true };

	DevicePolicyClient &client = GetDevicePolicyClient(handle.get());
	status = client.methodCall<bool>("Usb::getDebuggingState");
	EXPECT_EQ(true, status.get());

	status = client.methodCall<bool>("Usb::getTetheringState");
	EXPECT_EQ(true, status.get());

	status = client.methodCall<bool>("Usb::getClientState");
	EXPECT_EQ(true, status.get());
}
