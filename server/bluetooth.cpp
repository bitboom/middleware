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

#include <bluetooth.h>
#include <bluetooth-api.h>
#include <bluetooth_internal.h>
#include <klay/exception.h>
#include <klay/audit/logger.h>
#include <klay/dbus/connection.h>

#include "privilege.h"
#include "policy-builder.h"

#include "bluetooth.hxx"

#define POLICY_ENFORCING_FAILED(ret)                    \
	(((ret) == BLUETOOTH_DPM_RESULT_ACCESS_DENIED) ||   \
	 ((ret) == BLUETOOTH_DPM_RESULT_FAIL))

#define POLICY_IS_ALLOWED(enable)                       \
	((enable) ? BLUETOOTH_DPM_ALLOWED :                 \
				BLUETOOTH_DPM_RESTRICTED)

#define STATE_CHANGE_IS_ALLOWED(enable)                 \
	((enable) ? BLUETOOTH_DPM_BT_ALLOWED :              \
				BLUETOOTH_DPM_BT_RESTRICTED)


#define MOBILEAP_INTERFACE		\
	"org.tizen.MobileapAgent",	\
	"/MobileapAgent",			\
	"org.tizen.tethering"

namespace DevicePolicyManager {

namespace {

struct BluetoothPolicyContext {
	BluetoothPolicyContext(BluetoothPolicy* p, PolicyControlContext* c) :
		policy(p), context(c)
	{
	}

	BluetoothPolicy* policy;
	PolicyControlContext* context;
};

inline int __setModeChangeState(const bool enable)
{
	return bluetooth_dpm_set_allow_mode(STATE_CHANGE_IS_ALLOWED(enable));
}

inline int __setDesktopConnectivityState(const bool enable)
{
	return bluetooth_dpm_set_desktop_connectivity_state(POLICY_IS_ALLOWED(enable));
}

inline int __setPairingState(const bool enable)
{
	return bluetooth_dpm_set_pairing_state(POLICY_IS_ALLOWED(enable));
}

inline int __setDeviceRestriction(const bool enable)
{
	return bluetooth_dpm_activate_device_restriction(POLICY_IS_ALLOWED(!enable));
}

inline int __setUuidRestriction(const bool enable)
{
	return bluetooth_dpm_activate_uuid_restriction(POLICY_IS_ALLOWED(!enable));
}

void bluetoothAdapterStateChangedCallback(int result, bt_adapter_state_e state, void *user_data)
{
	if (state != BT_ADAPTER_ENABLED) {
		return;
	}

	BluetoothPolicyContext *bluetooth = (BluetoothPolicyContext *)user_data;
	PolicyControlContext &context = *bluetooth->context;

	int ret = __setModeChangeState(context.getPolicy("bluetooth"));
	if (POLICY_ENFORCING_FAILED(ret)) {
	}

	ret = __setDesktopConnectivityState(context.getPolicy("bluetooth-desktop-connectivity"));
	if (POLICY_ENFORCING_FAILED(ret)) {
	}

	ret = __setPairingState(context.getPolicy("bluetooth-pairing"));
	if (POLICY_ENFORCING_FAILED(ret)) {
	}

	ret = __setDeviceRestriction(context.getPolicy("bluetooth-device-restriction"));
	if (POLICY_ENFORCING_FAILED(ret)) {
	}

	ret = __setUuidRestriction(context.getPolicy("bluetooth-uuid-restriction"));
	if (POLICY_ENFORCING_FAILED(ret)) {
	}
}

std::vector<std::string> bluetoothNotifications = {
	"bluetooth",
	"bluetooth-tethering",
	"bluetooth-desktop-connectivity",
	"bluetooth-pairing",
	"bluetooth-uuid-restriction",
	"bluetooth-device-restriction"
};

} // namespace

BluetoothPolicy::BluetoothPolicy(PolicyControlContext& ctxt) :
	context(ctxt)
{
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setModeChangeState)(bool));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setDesktopConnectivityState)(bool));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setTetheringState)(bool));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setPairingState)(bool));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::addDeviceToBlacklist)(std::string));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::removeDeviceFromBlacklist)(std::string));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setDeviceRestriction)(bool));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::addUuidToBlacklist)(std::string));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::removeUuidFromBlacklist)(std::string));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setUuidRestriction)(bool));

	ctxt.registerNonparametricMethod(this, "", (bool)(BluetoothPolicy::getModeChangeState));
	ctxt.registerNonparametricMethod(this, "", (bool)(BluetoothPolicy::getDesktopConnectivityState));
	ctxt.registerNonparametricMethod(this, "", (bool)(BluetoothPolicy::getTetheringState));
	ctxt.registerNonparametricMethod(this, "", (bool)(BluetoothPolicy::getPairingState));
	ctxt.registerNonparametricMethod(this, "", (bool)(BluetoothPolicy::isDeviceRestricted));
	ctxt.registerNonparametricMethod(this, "", (bool)(BluetoothPolicy::isUuidRestricted));

	ctxt.createNotification(bluetoothNotifications);

	if (::bt_initialize() != BT_ERROR_NONE) {
		return;
	}

	if (::bt_adapter_set_state_changed_cb(bluetoothAdapterStateChangedCallback,
										  new BluetoothPolicyContext(this, &ctxt)) != BT_ERROR_NONE) {
		return;
	}
}

BluetoothPolicy::~BluetoothPolicy()
{
	bt_deinitialize();
}

int BluetoothPolicy::setModeChangeState(const bool enable)
{
	if (!SetPolicyAllowed(context, "bluetooth", enable)) {
		return 0;
	}

	int ret = __setModeChangeState(enable);
	if (POLICY_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::getModeChangeState()
{
	return context.getPolicy("bluetooth");
}

int BluetoothPolicy::setDesktopConnectivityState(const bool enable)
{
	if (!SetPolicyAllowed(context, "bluetooth-desktop-connectivity", enable)) {
		return 0;
	}

	int ret = __setDesktopConnectivityState(enable);
	if (POLICY_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::getDesktopConnectivityState()
{
	return context.getPolicy("bluetooth-desktop-connectivity");
}

int BluetoothPolicy::setPairingState(const bool enable)
{
	if (!SetPolicyAllowed(context, "bluetooth-pairing", enable)) {
		return 0;
	}

	int ret = __setPairingState(enable);
	if (POLICY_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::getPairingState()
{
	return context.getPolicy("bluetooth-pairing");
}


int BluetoothPolicy::addDeviceToBlacklist(const std::string& mac)
{
	int ret = bt_dpm_add_devices_to_blacklist(mac.c_str());
	if (POLICY_ENFORCING_FAILED(ret)) {
		return -1;
	}

	return ret;
}

int BluetoothPolicy::setTetheringState(bool enable)
{
	try {
		if (!SetPolicyAllowed(context, "bluetooth-tethering", enable)) {
			return 0;
		}

		dbus::Connection &systemDBus = dbus::Connection::getSystem();
		systemDBus.methodcall(MOBILEAP_INTERFACE,
							  "change_policy",
							  -1,
							  "",
							  "(sb)",
							  "bluetooth-tethering",
							  enable);
	} catch (runtime::Exception& e) {
		ERROR("Failed to change bluetooth tethering state");
		return -1;
	}

	return 0;
}

bool BluetoothPolicy::getTetheringState()
{
	return context.getPolicy("bluetooth-tethering");
}

int BluetoothPolicy::removeDeviceFromBlacklist(const std::string& mac)
{
	int ret = bt_dpm_remove_device_from_blacklist(mac.c_str());
	if (POLICY_ENFORCING_FAILED(ret)) {
		return -1;
	}

	return ret;
}

int BluetoothPolicy::setDeviceRestriction(const bool enable)
{
	if (SetPolicyEnabled(context, "bluetooth-device-restriction", enable)) {
		return 0;
	}

	int ret = __setDeviceRestriction(enable);
	if (POLICY_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::isDeviceRestricted()
{
	return context.getPolicy("bluetooth-device-restriction");
}

int BluetoothPolicy::addUuidToBlacklist(const std::string& uuid)
{
	int ret = bluetooth_dpm_add_uuids_to_blacklist(uuid.c_str());
	if (POLICY_ENFORCING_FAILED(ret)) {
		return -1;
	}

	return ret;
}

int BluetoothPolicy::removeUuidFromBlacklist(const std::string& uuid)
{
	int ret = bluetooth_dpm_remove_uuid_from_blacklist(uuid.c_str());
	if (POLICY_ENFORCING_FAILED(ret)) {
		return -1;
	}

	return ret;
}

int BluetoothPolicy::setUuidRestriction(const bool enable)
{
	if (SetPolicyEnabled(context, "bluetooth-uuid-restriction", enable)) {
		return 0;
	}

	int ret = __setUuidRestriction(enable);
	if (POLICY_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::isUuidRestricted()
{
	return context.getPolicy("bluetooth-uuid-restriction");
}

DEFINE_POLICY(BluetoothPolicy);

} // namespace DevicePolicyManager
