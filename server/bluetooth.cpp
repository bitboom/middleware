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

#define BT_ENFORCING_FAILED(ret)                        \
	(((ret) == BLUETOOTH_DPM_RESULT_ACCESS_DENIED) ||   \
	 ((ret) == BLUETOOTH_DPM_RESULT_FAIL))

#define POLICY_IS_ALLOWED(enable)                       \
	((enable) ? BLUETOOTH_DPM_ALLOWED :                 \
				BLUETOOTH_DPM_RESTRICTED)

#define STATE_CHANGE_IS_ALLOWED(enable)                 \
	((enable) ? BLUETOOTH_DPM_BT_ALLOWED :              \
				BLUETOOTH_DPM_BT_RESTRICTED)

namespace DevicePolicyManager {

namespace {

std::vector<std::string> bluetoothNotifications = {
	"bluetooth",
	"bluetooth-tethering",
	"bluetooth-desktop-connectivity",
	"bluetooth-pairing",
	"bluetooth-uuid-restriction",
	"bluetooth-device-restriction"
};

} // namespace

struct BluetoothPolicy::Private {
	Private(PolicyControlContext& ctx);
	~Private();

	int setModeChangeState(bool enable);
	bool getModeChangeState();
	int setDesktopConnectivityState(bool enable);
	bool getDesktopConnectivityState();
	int setPairingState(bool enable);
	bool getPairingState();
	int addDeviceToBlacklist(const std::string& mac);
	int setTetheringState(bool enable);
	bool getTetheringState();
	int removeDeviceFromBlacklist(const std::string& mac);
	int setDeviceRestriction(bool enable);
	bool isDeviceRestricted();
	int addUuidToBlacklist(const std::string& uuid);
	int removeUuidFromBlacklist(const std::string& uuid);
	int setUuidRestriction(bool enable);
	bool isUuidRestricted();

	static void onStateChanged(int result, bt_adapter_state_e state, void *user_data);

	PolicyControlContext& context;
};

BluetoothPolicy::Private::Private(PolicyControlContext& ctxt) :
	context(ctxt)
{
	if (::bt_initialize() != BT_ERROR_NONE) {
		return;
	}

	if (::bt_adapter_set_state_changed_cb(onStateChanged, this) != BT_ERROR_NONE) {
		return;
	}
}

BluetoothPolicy::Private::~Private()
{
	::bt_deinitialize();
}

int BluetoothPolicy::Private::setModeChangeState(bool enable)
{
	if (!SetPolicyAllowed(context, "bluetooth", enable)) {
		return 0;
	}

	int ret = bluetooth_dpm_set_allow_mode(STATE_CHANGE_IS_ALLOWED(enable));
	if (BT_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::Private::getModeChangeState()
{
	return context.getPolicy<int>("bluetooth");
}

int BluetoothPolicy::Private::setDesktopConnectivityState(bool enable)
{
	if (!SetPolicyAllowed(context, "bluetooth-desktop-connectivity", enable)) {
		return 0;
	}

	int ret = bluetooth_dpm_set_desktop_connectivity_state(POLICY_IS_ALLOWED(enable));
	if (BT_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::Private::getDesktopConnectivityState()
{
	return context.getPolicy<int>("bluetooth-desktop-connectivity");
}

int BluetoothPolicy::Private::setPairingState(bool enable)
{
	if (!SetPolicyAllowed(context, "bluetooth-pairing", enable)) {
		return 0;
	}

	int ret = bluetooth_dpm_set_pairing_state(POLICY_IS_ALLOWED(enable));
	if (BT_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::Private::getPairingState()
{
	return context.getPolicy<int>("bluetooth-pairing");
}

int BluetoothPolicy::Private::addDeviceToBlacklist(const std::string& mac)
{
	int ret = bt_dpm_add_devices_to_blacklist(mac.c_str());
	if (BT_ENFORCING_FAILED(ret)) {
		return -1;
	}

	return ret;
}

int BluetoothPolicy::Private::setTetheringState(bool enable)
{
	try {
		if (!SetPolicyAllowed(context, "bluetooth-tethering", enable)) {
			return 0;
		}
	} catch (runtime::Exception& e) {
		ERROR("Failed to change bluetooth tethering state");
		return -1;
	}

	return 0;
}

bool BluetoothPolicy::Private::getTetheringState()
{
	return context.getPolicy<int>("bluetooth-tethering");
}

int BluetoothPolicy::Private::removeDeviceFromBlacklist(const std::string& mac)
{
	int ret = bt_dpm_remove_device_from_blacklist(mac.c_str());
	if (BT_ENFORCING_FAILED(ret)) {
		return -1;
	}

	return ret;
}

int BluetoothPolicy::Private::setDeviceRestriction(bool enable)
{
	if (SetPolicyEnabled(context, "bluetooth-device-restriction", enable)) {
		return 0;
	}

	int ret = bluetooth_dpm_activate_device_restriction(POLICY_IS_ALLOWED(!enable));
	if (BT_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::Private::isDeviceRestricted()
{
	return context.getPolicy<int>("bluetooth-device-restriction");
}

int BluetoothPolicy::Private::addUuidToBlacklist(const std::string& uuid)
{
	int ret = bluetooth_dpm_add_uuids_to_blacklist(uuid.c_str());
	if (BT_ENFORCING_FAILED(ret)) {
		return -1;
	}

	return ret;
}

int BluetoothPolicy::Private::removeUuidFromBlacklist(const std::string& uuid)
{
	int ret = bluetooth_dpm_remove_uuid_from_blacklist(uuid.c_str());
	if (BT_ENFORCING_FAILED(ret)) {
		return -1;
	}

	return ret;
}

int BluetoothPolicy::Private::setUuidRestriction(bool enable)
{
	if (SetPolicyEnabled(context, "bluetooth-uuid-restriction", enable)) {
		return 0;
	}

	int ret = bluetooth_dpm_activate_uuid_restriction(POLICY_IS_ALLOWED(!enable));
	if (BT_ENFORCING_FAILED(ret))
		return -1;

	return 0;
}

bool BluetoothPolicy::Private::isUuidRestricted()
{
	return context.getPolicy<int>("bluetooth-uuid-restriction");
}

void BluetoothPolicy::Private::onStateChanged(int result, bt_adapter_state_e state, void *user_data)
{
	BluetoothPolicy::Private *policy = reinterpret_cast<BluetoothPolicy::Private *>(user_data);

	if (state != BT_ADAPTER_ENABLED) {
		return;
	}

	PolicyControlContext& context = policy->context;

	int ret = bluetooth_dpm_set_allow_mode(STATE_CHANGE_IS_ALLOWED(context.getPolicy<int>("bluetooth")));
	if (BT_ENFORCING_FAILED(ret)) {
	}

	ret = bluetooth_dpm_set_desktop_connectivity_state(POLICY_IS_ALLOWED(context.getPolicy<int>("bluetooth-desktop-connectivity")));
	if (BT_ENFORCING_FAILED(ret)) {
	}

	ret = bluetooth_dpm_set_pairing_state(POLICY_IS_ALLOWED(context.getPolicy<int>("bluetooth-pairing")));
	if (BT_ENFORCING_FAILED(ret)) {
	}

	ret = bluetooth_dpm_activate_device_restriction(POLICY_IS_ALLOWED(!context.getPolicy<int>("bluetooth-device-restriction")));
	if (BT_ENFORCING_FAILED(ret)) {
	}

	ret = bluetooth_dpm_activate_uuid_restriction(POLICY_IS_ALLOWED(!context.getPolicy<int>("bluetooth-uuid-restriction")));
	if (BT_ENFORCING_FAILED(ret)) {
	}
}

BluetoothPolicy::BluetoothPolicy(BluetoothPolicy&& rhs) = default;
BluetoothPolicy& BluetoothPolicy::operator=(BluetoothPolicy&& rhs) = default;

BluetoothPolicy::BluetoothPolicy(const BluetoothPolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

BluetoothPolicy& BluetoothPolicy::operator=(const BluetoothPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

BluetoothPolicy::BluetoothPolicy(PolicyControlContext& context) :
	pimpl(new Private(context))
{
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setModeChangeState)(bool));
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setDesktopConnectivityState)(bool));
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setTetheringState)(bool));
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setPairingState)(bool));
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::addDeviceToBlacklist)(std::string));
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::removeDeviceFromBlacklist)(std::string));
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setDeviceRestriction)(bool));
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::addUuidToBlacklist)(std::string));
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::removeUuidFromBlacklist)(std::string));
	context.expose(this, DPM_PRIVILEGE_BLUETOOTH, (int)(BluetoothPolicy::setUuidRestriction)(bool));

	context.expose(this, "", (bool)(BluetoothPolicy::getModeChangeState)());
	context.expose(this, "", (bool)(BluetoothPolicy::getDesktopConnectivityState)());
	context.expose(this, "", (bool)(BluetoothPolicy::getTetheringState)());
	context.expose(this, "", (bool)(BluetoothPolicy::getPairingState)());
	context.expose(this, "", (bool)(BluetoothPolicy::isDeviceRestricted)());
	context.expose(this, "", (bool)(BluetoothPolicy::isUuidRestricted)());

	context.createNotification(bluetoothNotifications);
}

BluetoothPolicy::~BluetoothPolicy()
{
}

int BluetoothPolicy::setModeChangeState(bool enable)
{
	return pimpl->setModeChangeState(enable);
}

bool BluetoothPolicy::getModeChangeState()
{
	return pimpl->getModeChangeState();
}

int BluetoothPolicy::setDesktopConnectivityState(bool enable)
{
	return pimpl->setDesktopConnectivityState(enable);
}

bool BluetoothPolicy::getDesktopConnectivityState()
{
	return pimpl->getDesktopConnectivityState();
}

int BluetoothPolicy::setPairingState(bool enable)
{
	return pimpl->setPairingState(enable);
}

bool BluetoothPolicy::getPairingState()
{
	return pimpl->getPairingState();
}

int BluetoothPolicy::addDeviceToBlacklist(const std::string& mac)
{
	return pimpl->addDeviceToBlacklist(mac);
}

int BluetoothPolicy::setTetheringState(bool enable)
{
	return pimpl->setTetheringState(enable);
}

bool BluetoothPolicy::getTetheringState()
{
	return pimpl->getTetheringState();
}

int BluetoothPolicy::removeDeviceFromBlacklist(const std::string& mac)
{
	return pimpl->removeDeviceFromBlacklist(mac);
}

int BluetoothPolicy::setDeviceRestriction(bool enable)
{
	return pimpl->setDeviceRestriction(enable);
}

bool BluetoothPolicy::isDeviceRestricted()
{
	return pimpl->isDeviceRestricted();
}

int BluetoothPolicy::addUuidToBlacklist(const std::string& uuid)
{
	return pimpl->addUuidToBlacklist(uuid);
}

int BluetoothPolicy::removeUuidFromBlacklist(const std::string& uuid)
{
	return pimpl->removeUuidFromBlacklist(uuid);
}

int BluetoothPolicy::setUuidRestriction(bool enable)
{
	return pimpl->setUuidRestriction(enable);
}

bool BluetoothPolicy::isUuidRestricted()
{
	return pimpl->isUuidRestricted();
}

DEFINE_POLICY(BluetoothPolicy);

} // namespace DevicePolicyManager
