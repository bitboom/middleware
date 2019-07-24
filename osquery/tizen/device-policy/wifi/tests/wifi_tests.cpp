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

#include "../wifi.h"

#include <memory>

class WifiPolicyTests : public testing::Test {};

TEST_F(WifiPolicyTests, Wifi) {
	/// device_policy_manager_h is void* type
	std::shared_ptr<void> handle(dpm_manager_create(), dpm_manager_destroy);
	ASSERT_TRUE(handle != nullptr);

	bool isAllowed = false;
	auto ret = dpm_wifi_get_state(handle.get(), &isAllowed);
	EXPECT_EQ(ret, DPM_ERROR_NONE);

	int id = 0;
	auto callback = [](const char* name, const char* state, void* user_data) {
		VLOG(1) << name << " policy changed to -> " << state;
	};
	ret = dpm_add_policy_changed_cb(handle.get(), "wifi", callback, NULL, &id);
	EXPECT_EQ(ret, DPM_ERROR_NONE);

	ret = dpm_remove_policy_changed_cb(handle.get(), id);
	EXPECT_EQ(ret, DPM_ERROR_NONE);
}

TEST_F(WifiPolicyTests, Hotspot) {
	/// device_policy_manager_h is void* type
	std::shared_ptr<void> handle(dpm_manager_create(), dpm_manager_destroy);
	ASSERT_TRUE(handle != nullptr);

	bool isAllowed = false;
	auto ret = dpm_wifi_get_hotspot_state(handle.get(), &isAllowed);
	EXPECT_EQ(ret, DPM_ERROR_NONE);

	int id = 0;
	auto callback = [](const char* name, const char* state, void* user_data) {
		VLOG(1) << name << " policy changed to -> " << state;
	};
	ret = dpm_add_policy_changed_cb(handle.get(), "wifi_hotspot", callback, NULL, &id);
	EXPECT_EQ(ret, DPM_ERROR_NONE);

	ret = dpm_remove_policy_changed_cb(handle.get(), id);
	EXPECT_EQ(ret, DPM_ERROR_NONE);
}

TEST_F(WifiPolicyTests, ProfileChange) {
	/// device_policy_manager_h is void* type
	std::shared_ptr<void> handle(dpm_manager_create(), dpm_manager_destroy);
	ASSERT_TRUE(handle != nullptr);

	bool isAllowed = false;
	auto ret = dpm_wifi_is_profile_change_restricted(handle.get(), &isAllowed);
	EXPECT_EQ(ret, DPM_ERROR_NONE);

	int id = 0;
	auto callback = [](const char* name, const char* state, void* user_data) {
		VLOG(1) << name << " policy changed to -> " << state;
	};
	ret = dpm_add_policy_changed_cb(handle.get(), "wifi_profile_change", callback, NULL, &id);
	EXPECT_EQ(ret, DPM_ERROR_NONE);

	ret = dpm_remove_policy_changed_cb(handle.get(), id);
	EXPECT_EQ(ret, DPM_ERROR_NONE);
}
