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
#include "policy-model.h"

#include "bluetooth.hxx"

#ifdef BLUETOOTH_MAX_DPM_LIST
#define BLUETOOTH_DPM_SUPPORTED
#endif

#ifdef BLUETOOTH_DPM_SUPPORTED
#define BT_FAILED(ret)                                  \
	(((ret) == BLUETOOTH_DPM_RESULT_ACCESS_DENIED) ||   \
	 ((ret) == BLUETOOTH_DPM_RESULT_FAIL))

#define POLICY_IS_ALLOWED(enable)                       \
	((enable) ? BLUETOOTH_DPM_ALLOWED :                 \
				BLUETOOTH_DPM_RESTRICTED)

#define STATE_CHANGE_IS_ALLOWED(enable)                 \
	((enable) ? BLUETOOTH_DPM_BT_ALLOWED :              \
				BLUETOOTH_DPM_BT_RESTRICTED)
#endif

namespace DevicePolicyManager {

namespace {

inline int canonicalize(int value)
{
	return -value;
}

} // namespace

class BluetoothStateEnforceModel : public BaseEnforceModel {
public:
	BluetoothStateEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool enable)
	{
#ifdef BLUETOOTH_DPM_SUPPORTED
		int ret = bluetooth_dpm_set_allow_mode(STATE_CHANGE_IS_ALLOWED(enable));
		if (!BT_FAILED(ret)) {
			notify(enable);
			return true;
		}
#endif
		return false;
	}
};

class BluetoothConnectivityEnforceModel : public BaseEnforceModel {
public:
	BluetoothConnectivityEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool enable)
	{
#ifdef BLUETOOTH_DPM_SUPPORTED
		int ret = bluetooth_dpm_set_desktop_connectivity_state(POLICY_IS_ALLOWED(enable));
		if (!BT_FAILED(ret)) {
			notify(enable == 0 ? "disallowed" : "allowed");
			return true;
		}
#endif
		return false;
	}
};

class BluetoothUuidEnforceModel : public BaseEnforceModel {
public:
	BluetoothUuidEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool enable)
	{
#ifdef BLUETOOTH_DPM_SUPPORTED
		enable = canonicalize(enable);
		int ret = bluetooth_dpm_activate_uuid_restriction(POLICY_IS_ALLOWED(!enable));
		if (!BT_FAILED(ret)) {
			notify(enable == 0 ? "enabled" : "disabled");
			return true;
		}
#endif
		return false;
	}
};

class BluetoothDeviceEnforceModel : public BaseEnforceModel {
public:
	BluetoothDeviceEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool enable)
	{
#ifdef BLUETOOTH_DPM_SUPPORTED
		enable = canonicalize(enable);
		int ret = bluetooth_dpm_activate_device_restriction(POLICY_IS_ALLOWED(!enable));
		if (!BT_FAILED(ret)) {
			notify(enable == 0 ? "enabled" : "disabled");
			return true;
		}
#endif
		return false;
	}
};

class BluetoothPairingEnforceModel : public BaseEnforceModel {
public:
	BluetoothPairingEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool enable)
	{
#ifdef BLUETOOTH_DPM_SUPPORTED
		int ret = bluetooth_dpm_set_pairing_state(POLICY_IS_ALLOWED(enable));
		if (!BT_FAILED(ret)) {
			notify(enable == 0 ? "disallowed" : "allowed");
			return true;
		}
#endif
		return false;
	}
};

typedef GlobalPolicy<int, BluetoothStateEnforceModel> BluetoothStatePolicy;
typedef GlobalPolicy<int, BluetoothConnectivityEnforceModel> BluetoothConnectivityPolicy;
typedef GlobalPolicy<int, BluetoothUuidEnforceModel> BluetoothUuidRestrictionPolicy;
typedef GlobalPolicy<int, BluetoothDeviceEnforceModel> BluetoothDeviceRestrictionPolicy;
typedef GlobalPolicy<int, BluetoothPairingEnforceModel> BluetoothPairingPolicy;
typedef GlobalPolicy<int> BluetoothTetheringPolicy;

struct BluetoothPolicy::Private : public PolicyHelper {
	Private(PolicyControlContext& ctx);
	~Private();

	static void onStateChanged(int result, bt_adapter_state_e state, void *user_data);

	BluetoothStatePolicy             modeChange{context, "bluetooth"};
	BluetoothConnectivityPolicy      desktopConnectivity{context, "bluetooth-desktop-connectivity"};
	BluetoothUuidRestrictionPolicy   uuidRestriction{context, "bluetooth-uuid-restriction"};
	BluetoothDeviceRestrictionPolicy deviceRestriction{context, "bluetooth-device-restriction"};
	BluetoothPairingPolicy           pairing{context, "bluetooth-pairing"};
	BluetoothTetheringPolicy         tethering{context, "bluetooth-tethering"};
};

BluetoothPolicy::Private::Private(PolicyControlContext& ctxt) :
	PolicyHelper(ctxt)
{
	if (::bt_initialize() != BT_ERROR_NONE) {
		ERROR("Bluetooth framework was not initilaized");
		return;
	}

	if (::bt_adapter_set_state_changed_cb(onStateChanged, this) != BT_ERROR_NONE) {
		ERROR("Failed to register Bluetooth callback");
		return;
	}
}

BluetoothPolicy::Private::~Private()
{
	::bt_deinitialize();
}

void BluetoothPolicy::Private::onStateChanged(int result, bt_adapter_state_e state, void *user_data)
{
	BluetoothPolicy::Private *pimpl = reinterpret_cast<BluetoothPolicy::Private *>(user_data);
	if (pimpl != nullptr && state == BT_ADAPTER_ENABLED) {
		pimpl->modeChange.enforce();
		pimpl->desktopConnectivity.enforce();
		pimpl->pairing.enforce();
		pimpl->deviceRestriction.enforce();
		pimpl->uuidRestriction.enforce();
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
}

BluetoothPolicy::~BluetoothPolicy()
{
}

int BluetoothPolicy::setModeChangeState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->modeChange, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool BluetoothPolicy::getModeChangeState()
{
	return pimpl->getPolicy(pimpl->modeChange);
}

int BluetoothPolicy::setDesktopConnectivityState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->desktopConnectivity, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool BluetoothPolicy::getDesktopConnectivityState()
{
	return pimpl->getPolicy(pimpl->desktopConnectivity);
}

int BluetoothPolicy::setPairingState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->pairing, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool BluetoothPolicy::getPairingState()
{
	return pimpl->getPolicy(pimpl->pairing);
}

int BluetoothPolicy::addDeviceToBlacklist(const std::string& mac)
{
	return -1;
}

int BluetoothPolicy::setTetheringState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->tethering, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool BluetoothPolicy::getTetheringState()
{
	return pimpl->getPolicy(pimpl->tethering);
}

int BluetoothPolicy::removeDeviceFromBlacklist(const std::string& mac)
{
	return -1;
}

int BluetoothPolicy::setDeviceRestriction(bool enable)
{
	try {
		enable = canonicalize(enable);
		pimpl->setPolicy(pimpl->deviceRestriction, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool BluetoothPolicy::isDeviceRestricted()
{
	int value = pimpl->getPolicy(pimpl->deviceRestriction);
	return canonicalize(value);
}

int BluetoothPolicy::addUuidToBlacklist(const std::string& uuid)
{
	return -1;
}

int BluetoothPolicy::removeUuidFromBlacklist(const std::string& uuid)
{
	return -1;
}

int BluetoothPolicy::setUuidRestriction(bool enable)
{
	try {
		enable = canonicalize(enable);
		pimpl->setPolicy(pimpl->uuidRestriction, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool BluetoothPolicy::isUuidRestricted()
{
	int value = pimpl->getPolicy(pimpl->uuidRestriction);
	return canonicalize(value);
}

DEFINE_POLICY(BluetoothPolicy);

} // namespace DevicePolicyManager
