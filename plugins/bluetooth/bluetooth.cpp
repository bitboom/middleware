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

#include <bluetooth.h>
#include <bluetooth-api.h>
#include <bluetooth_internal.h>

#include <policyd/pil/policy-context.h>
#include <policyd/pil/policy-model.h>
#include <policyd/pil/policy-storage.h>
#include <policyd/pil/policy-event.h>

#include "../dlog.h"

#define BT_FAILED(ret)                                       \
	(((int)(ret) == BLUETOOTH_DPM_RESULT_ACCESS_DENIED) ||   \
	 ((int)(ret) == BLUETOOTH_DPM_RESULT_FAIL))

#define POLICY_IS_ALLOWED(enable)                            \
	((int)(enable) ? BLUETOOTH_DPM_ALLOWED :                 \
					 BLUETOOTH_DPM_RESTRICTED)

#define STATE_CHANGE_IS_ALLOWED(enable)                      \
	((int)(enable) ? BLUETOOTH_DPM_BT_ALLOWED :              \
					 BLUETOOTH_DPM_BT_RESTRICTED)

namespace {

inline int canonicalize(int value)
{
	return -value;
}

} // namespace

class ModeChange : public GlobalPolicy<DataSetInt> {
public:
	ModeChange() : GlobalPolicy("bluetooth")
	{
		PolicyEventNotifier::create("bluetooth");
	}

	bool apply(const DataType& value)
	{
		int ret = bluetooth_dpm_set_allow_mode(STATE_CHANGE_IS_ALLOWED(value));
		if (!BT_FAILED(ret)) {
			int enable = value;
			PolicyEventNotifier::emit("bluetooth", enable ? "allowed" : "disallowed");
			return true;
		}
		return false;
	}
};

class DesktopConnectivity : public GlobalPolicy<DataSetInt> {
public:
	DesktopConnectivity() : GlobalPolicy("bluetooth-desktop-connectivity")
	{
		PolicyEventNotifier::create("bluetooth_desktop_connectivity");
	}

	bool apply(const DataType & value)
	{
		int ret = bluetooth_dpm_set_desktop_connectivity_state(POLICY_IS_ALLOWED(value));
		if (!BT_FAILED(ret)) {
			int enable = value;
			PolicyEventNotifier::emit("bluetooth_desktop_connectivity",
									  enable ? "allowed" : "disallowed");
			return true;
		}
		return false;
	}
};

class Pairing: public GlobalPolicy<DataSetInt> {
public:
	Pairing() : GlobalPolicy("bluetooth-pairing")
	{
		PolicyEventNotifier::create("bluetooth_pairing");
	}

	bool apply(const DataType& value)
	{
		int ret = bluetooth_dpm_set_pairing_state(POLICY_IS_ALLOWED(value));
		if (!BT_FAILED(ret)) {
			int enable = value;
			PolicyEventNotifier::emit("bluetooth_pairing",
									  enable ? "allowed" : "disallowed");
			return true;
		}
		return false;
	}
};

class Tethering: public GlobalPolicy<DataSetInt> {
public:
	Tethering() : GlobalPolicy("bluetooth-tethering")
	{
		PolicyEventNotifier::create("bluetooth_tethering");
	}

	bool apply(const DataType& value)
	{
		int enable = value;
		PolicyEventNotifier::emit("bluetooth_tethering",
								  enable ? "allowed" : "disallowed");
		return true;
	}
};

class Bluetooth : public AbstractPolicyProvider {
public:
	Bluetooth();
	~Bluetooth();

	int setModeChangeState(bool enable);
	bool getModeChangeState();
	int setDesktopConnectivityState(bool enable);
	bool getDesktopConnectivityState();
	int setTetheringState(bool enable);
	bool getTetheringState();
	int setPairingState(bool enable);
	bool getPairingState();

private:
	static void onStateChanged(int result, bt_adapter_state_e state, void *user_data);

private:
	ModeChange          modeChange;
	DesktopConnectivity connectivity;
	Pairing             pairing;
	Tethering          tethering;
};

Bluetooth::Bluetooth()
{
	if (::bt_initialize() != BT_ERROR_NONE) {
		ERROR(PLUGINS, "Bluetooth framework was not initilaized");
		return;
	}

	if (::bt_adapter_set_state_changed_cb(onStateChanged, this) != BT_ERROR_NONE) {
		ERROR(PLUGINS, "Failed to register Bluetooth callback");
		return;
	}
}

Bluetooth::~Bluetooth()
{
	::bt_deinitialize();
}

void Bluetooth::onStateChanged(int result, bt_adapter_state_e state, void *user_data)
{
	Bluetooth *pimpl = reinterpret_cast<Bluetooth *>(user_data);
	if (pimpl != nullptr && state == BT_ADAPTER_ENABLED) {
//		pimpl->modeChange.apply();
//		pimpl->desktopConnectivity.apply();
//		pimpl->pairing.apply();
//		pimpl->deviceRestriction.enforce();
//		pimpl->uuidRestriction.enforce();
	}
}

int Bluetooth::setModeChangeState(bool enable)
{
	try {
		modeChange.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, "Exception: " << e.what());
		return -1;
	}

	return 0;
}

bool Bluetooth::getModeChangeState()
{
	return modeChange.get();
}

int Bluetooth::setDesktopConnectivityState(bool enable)
{
	try {
		connectivity.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, "Exception: " << e.what());
		return -1;
	}

	return 0;
}

bool Bluetooth::getDesktopConnectivityState()
{
	return connectivity.get();
}

int Bluetooth::setPairingState(bool enable)
{
	try {
		pairing.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, "Exception: " << e.what());
		return -1;
	}

	return 0;
}

bool Bluetooth::getPairingState()
{
	return pairing.get();
}

int Bluetooth::setTetheringState(bool enable)
{
	try {
		tethering.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, "Exception " << e.what());
		return -1;
	}

	return 0;
}

bool Bluetooth::getTetheringState()
{
	return tethering.get();
}


extern "C" {

#define PRIVILEGE "http://tizen.org/privilege/dpm.bluetooth"

AbstractPolicyProvider *PolicyFactory(PolicyControlContext& context)
{
	INFO(PLUGINS, "Bluetooth plugin loaded");
	Bluetooth *policy = new Bluetooth();

	context.expose(policy, PRIVILEGE, (int)(Bluetooth::setModeChangeState)(bool));
	context.expose(policy, PRIVILEGE, (int)(Bluetooth::setDesktopConnectivityState)(bool));
	context.expose(policy, PRIVILEGE, (int)(Bluetooth::setTetheringState)(bool));
	context.expose(policy, PRIVILEGE, (int)(Bluetooth::setPairingState)(bool));

	context.expose(policy, "", (bool)(Bluetooth::getModeChangeState)());
	context.expose(policy, "", (bool)(Bluetooth::getDesktopConnectivityState)());
	context.expose(policy, "", (bool)(Bluetooth::getTetheringState)());
	context.expose(policy, "", (bool)(Bluetooth::getPairingState)());

	return policy;
}

} // extern "C"
