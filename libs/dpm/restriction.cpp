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

	try {
		return restriction.setCameraState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_camera_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		*is_allowed = restriction.getCameraState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_microphone_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		return restriction.setMicrophoneState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_microphone_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		*is_allowed = restriction.getMicrophoneState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_location_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	LocationPolicy location = client.createPolicyInterface<LocationPolicy>();

	try {
		return location.setLocationState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_location_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	LocationPolicy location = client.createPolicyInterface<LocationPolicy>();

	try {
		*is_allowed = location.getLocationState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_usb_debugging_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		return restriction.setUsbDebuggingState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_usb_debugging_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		*is_allowed = restriction.getUsbDebuggingState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_usb_tethering_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		return restriction.setUsbTetheringState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_usb_tethering_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		*is_allowed = restriction.getUsbTetheringState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_external_storage_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		return restriction.setExternalStorageState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_external_storage_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		*is_allowed = restriction.getExternalStorageState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_clipboard_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		return restriction.setClipboardState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_clipboard_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		*is_allowed = restriction.getClipboardState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_wifi_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		return wifi.setState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_wifi_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		*is_allowed = wifi.getState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_wifi_hotspot_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		return wifi.setHotspotState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_wifi_hotspot_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	WifiPolicy wifi = client.createPolicyInterface<WifiPolicy>();

	try {
		*is_allowed = wifi.getHotspotState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_bluetooth_tethering_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();

	try {
		return bluetooth.setTetheringState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_bluetooth_tethering_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();

	try {
		*is_allowed = bluetooth.getTetheringState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_bluetooth_mode_change_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();

	try {
		return bluetooth.setModeChangeState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_bluetooth_mode_change_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();

	try {
		*is_allowed = bluetooth.getModeChangeState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_bluetooth_desktop_connectivity_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();

	try {
		return bluetooth.setDesktopConnectivityState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_bluetooth_desktop_connectivity_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();

	try {
		*is_allowed = bluetooth.getDesktopConnectivityState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_bluetooth_pairing_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();

	try {
		return bluetooth.setPairingState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_bluetooth_pairing_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	BluetoothPolicy bluetooth = client.createPolicyInterface<BluetoothPolicy>();

	try {
		*is_allowed = bluetooth.getPairingState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_popimap_email_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		return restriction.setPopImapEmailState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_popimap_email_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		*is_allowed = restriction.getPopImapEmailState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_messaging_state(device_policy_manager_h handle, const char *sim_id, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		return restriction.setMessagingState(sim_id, allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_messaging_state(device_policy_manager_h handle, const char *sim_id, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		*is_allowed = restriction.getMessagingState(sim_id);
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_restriction_set_browser_state(device_policy_manager_h handle, int allow)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		return restriction.setBrowserState(allow);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_restriction_get_browser_state(device_policy_manager_h handle, int *is_allowed)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_allowed, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	RestrictionPolicy restriction = client.createPolicyInterface<RestrictionPolicy>();

	try {
		*is_allowed = restriction.getBrowserState();
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}
