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

#include <policyd/pil/global-policy.h>
#include <policyd/pil/policy-storage.h>
#include <policyd/pil/policy-event.h>

#include <memory>

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

class ModeChange : public GlobalPolicy {
public:
	ModeChange() : GlobalPolicy("bluetooth")
	{
		PolicyEventNotifier::create("bluetooth");
	}

	bool apply(const DataSetInt& value, uid_t)
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

class DesktopConnectivity : public GlobalPolicy {
public:
	DesktopConnectivity() : GlobalPolicy("bluetooth-desktop-connectivity")
	{
		PolicyEventNotifier::create("bluetooth_desktop_connectivity");
	}

	bool apply(const DataSetInt & value, uid_t)
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

class Pairing: public GlobalPolicy {
public:
	Pairing() : GlobalPolicy("bluetooth-pairing")
	{
		PolicyEventNotifier::create("bluetooth_pairing");
	}

	bool apply(const DataSetInt& value, uid_t)
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

class Tethering: public GlobalPolicy {
public:
	Tethering() : GlobalPolicy("bluetooth-tethering")
	{
		PolicyEventNotifier::create("bluetooth_tethering");
	}

	bool apply(const DataSetInt& value, uid_t)
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

private:
	static void onStateChanged(int result, bt_adapter_state_e state, void *user_data);
};

Bluetooth::Bluetooth()
{
	if (::bt_initialize() != BT_ERROR_NONE) {
		ERROR(PLUGINS,"Bluetooth framework was not initilaized");
		return;
	}

	if (::bt_adapter_set_state_changed_cb(onStateChanged, this) != BT_ERROR_NONE) {
		ERROR(PLUGINS,"Failed to register Bluetooth callback");
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
