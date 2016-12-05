/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "restriction.h"
#include "restriction.hxx"
#include "location.hxx"
#include "bluetooth.hxx"
#include "wifi.hxx"

#include "debug.h"
#include "policy-client.h"

using namespace DevicePolicyManager;

EXPORT_API int dpm_restriction_set_camera_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	return restriction.setCameraState(allow);
}

EXPORT_API int dpm_restriction_get_camera_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	int ret = restriction.getCameraState();
	if (ret < 0) {
		return -1;
	}

	*is_allowed = ret;
	return 0;
}

EXPORT_API int dpm_restriction_set_microphone_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	return restriction.setMicrophoneState(allow);
}

EXPORT_API int dpm_restriction_get_microphone_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	int ret = restriction.getMicrophoneState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return 0;
}

EXPORT_API int dpm_restriction_set_location_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	LocationPolicy location = client.createPolicyInterface<LocationPolicy>();
	return location.setLocationState(allow);
}

EXPORT_API int dpm_restriction_get_location_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	LocationPolicy location = client.createPolicyInterface<LocationPolicy>();
	int ret = location.getLocationState();
	if (ret < 0) {
		return -1;
	}

	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_usb_debugging_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	return restriction.setUsbDebuggingState(allow);
}

EXPORT_API int dpm_restriction_get_usb_debugging_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	int ret = restriction.getUsbDebuggingState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_usb_tethering_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	return restriction.setUsbTetheringState(allow);
}

EXPORT_API int dpm_restriction_get_usb_tethering_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	int ret = restriction.getUsbTetheringState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_external_storage_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	return restriction.setExternalStorageState(allow);
}

EXPORT_API int dpm_restriction_get_external_storage_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	int ret = restriction.getExternalStorageState();
	if (ret < 0) {
		return 0;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_clipboard_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	return restriction.setClipboardState(allow);
}

EXPORT_API int dpm_restriction_get_clipboard_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	int ret = restriction.getClipboardState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_wifi_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();
	return wifi.setState(allow);
}

EXPORT_API int dpm_restriction_get_wifi_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();
	int ret = wifi.getState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_wifi_hotspot_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();
	return  wifi.setHotspotState(allow);
}

EXPORT_API int dpm_restriction_get_wifi_hotspot_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();
	int ret = wifi.getHotspotState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_bluetooth_tethering_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();
	return bluetooth.setTetheringState(allow);
}

EXPORT_API int dpm_restriction_get_bluetooth_tethering_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();
	int ret = bluetooth.getTetheringState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_bluetooth_mode_change_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();
	return bluetooth.setModeChangeState(allow);
}

EXPORT_API int dpm_restriction_get_bluetooth_mode_change_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();
	int ret = bluetooth.getModeChangeState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_bluetooth_desktop_connectivity_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();
	return bluetooth.setDesktopConnectivityState(allow);
}

EXPORT_API int dpm_restriction_get_bluetooth_desktop_connectivity_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();
	int ret = bluetooth.getDesktopConnectivityState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_bluetooth_pairing_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();
	return bluetooth.setPairingState(allow);
}

EXPORT_API int dpm_restriction_get_bluetooth_pairing_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();
	int ret = bluetooth.getPairingState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_popimap_email_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	return restriction.setPopImapEmailState(allow);
}

EXPORT_API int dpm_restriction_get_popimap_email_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	int ret = restriction.getPopImapEmailState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_messaging_state(device_policy_manager_h handle, const char *sim_id, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	return restriction.setMessagingState(sim_id, allow);
}

EXPORT_API int dpm_restriction_get_messaging_state(device_policy_manager_h handle, const char *sim_id, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	int ret = restriction.getMessagingState(sim_id);
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_browser_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	return restriction.setBrowserState(allow);
}

EXPORT_API int dpm_restriction_get_browser_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();
	int ret = restriction.getBrowserState();
	if (ret < 0) {
		return -1;
	}
	*is_allowed = ret;
	return DPM_ERROR_NONE;
}
