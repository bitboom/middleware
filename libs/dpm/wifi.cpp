/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "wifi.h"
#include "wifi.hxx"

#include "debug.h"
#include "policy-client.h"

using namespace DevicePolicyManager;

EXPORT_API int dpm_wifi_set_profile_change_restriction(device_policy_manager_h handle, int enable)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		return wifi.setProfileChangeRestriction(enable);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_wifi_is_profile_change_restricted(device_policy_manager_h handle, int *enable)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(enable, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		*enable = wifi.isProfileChangeRestricted();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_wifi_set_network_access_restriction(device_policy_manager_h handle, int enable)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		return wifi.setNetworkAccessRestriction(enable);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_wifi_is_network_access_restricted(device_policy_manager_h handle, int *enable)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(enable, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		*enable = wifi.isNetworkAccessRestricted();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_wifi_add_ssid_to_blocklist(device_policy_manager_h handle, const char* ssid)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(ssid, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		return wifi.addSsidToBlocklist(ssid);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_wifi_remove_ssid_from_blocklist(device_policy_manager_h handle, const char* ssid)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(ssid, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		return wifi.removeSsidFromBlocklist(ssid);
	} catch (...) {
		return -1;
	}
}
