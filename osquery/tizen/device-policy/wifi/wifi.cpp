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
#include <tizen.h>
#include <tizen_type.h>

#include <dpm/pil/policy-client.h>

#include "wifi.h"

#define RET_ON_FAILURE(cond, ret) \
{                                 \
	if (!(cond))                  \
		return (ret);             \
}

EXPORT_API int dpm_wifi_set_state(device_policy_manager_h handle, bool allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient &client = GetDevicePolicyClient(handle);

	try {
		Status<int> status { -1 };
		status = client.methodCall<int>("Wifi::setState", allow);
		return status.get();
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_wifi_get_state(device_policy_manager_h handle, bool *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient &client = GetDevicePolicyClient(handle);

    try {
		Status<bool> status { true };
		status = client.methodCall<bool>("Wifi::getState");
		*is_allowed = status.get();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_wifi_set_hotspot_state(device_policy_manager_h handle, bool allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient &client = GetDevicePolicyClient(handle);

	try {
		Status<int> status { -1 };
		status = client.methodCall<int>("Wifi::setHotspotState", allow);
		return status.get();
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_wifi_get_hotspot_state(device_policy_manager_h handle, bool *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient &client = GetDevicePolicyClient(handle);

	try {
		Status<bool> status { true };
		status = client.methodCall<bool>("Wifi::getHotspotState");
		*is_allowed = status.get();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_wifi_set_profile_change_restriction(device_policy_manager_h handle, bool enable)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient &client = GetDevicePolicyClient(handle);

	try {
		Status<int> status { -1 };
		status = client.methodCall<int>("Wifi::setProfileChangeRestriction", enable);
		return status.get();
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_wifi_is_profile_change_restricted(device_policy_manager_h handle, bool *enable)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(enable, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient &client = GetDevicePolicyClient(handle);

	try {
		Status<bool> status { false };
		status = client.methodCall<bool>("Wifi::isProfileChangeRestricted");
		*enable = status.get();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}
