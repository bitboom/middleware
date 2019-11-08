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

#include <vist/sdk/policy-model.hpp>
#include <vist/sdk/policy-provider.hpp>

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

using namespace vist::policy;

class ModeChange : public PolicyModel {
public:
	ModeChange() : PolicyModel("bluetooth", PolicyValue(1)) {}

	void onChanged(const PolicyValue& value) override
	{
		int ret = ::bluetooth_dpm_set_allow_mode(STATE_CHANGE_IS_ALLOWED(value));
		if (BT_FAILED(ret))
			throw std::runtime_error("Failed to set bluetooth.");
	}
};

class DesktopConnectivity : public PolicyModel {
public:
	DesktopConnectivity() :
		PolicyModel("bluetooth-desktop-connectivity", PolicyValue(1)) {}

	void onChanged(const PolicyValue& value) override
	{
		int ret = ::bluetooth_dpm_set_desktop_connectivity_state(POLICY_IS_ALLOWED(value));
		if (BT_FAILED(ret))
			throw std::runtime_error("Failed to set bt_desktop_connectivity.");
	}
};

class Pairing: public PolicyModel {
public:
	Pairing() : PolicyModel("bluetooth-pairing", PolicyValue(1)) {}

	void onChanged(const PolicyValue& value) override
	{
		int ret = ::bluetooth_dpm_set_pairing_state(POLICY_IS_ALLOWED(value));
		if (BT_FAILED(ret))
			throw std::runtime_error("Failed to set bt_pairing.");
	}
};

class Tethering: public PolicyModel {
public:
	Tethering() : PolicyModel("bluetooth-tethering", PolicyValue(1)) {}

	void onChanged(const PolicyValue&) {}
};

class Bluetooth : public PolicyProvider {
public:
	Bluetooth(const std::string& name) : PolicyProvider(name)
	{
		if (::bt_initialize() != BT_ERROR_NONE)
			ERROR(PLUGINS,"Bluetooth framework was not initilaized");
	}

	~Bluetooth()
	{
		::bt_deinitialize();
	}
};

// TODO(Sangwan): Add privilege to provider
#define PRIVILEGE "http://tizen.org/privilege/dpm.bluetooth"

extern "C" PolicyProvider* PolicyFactory()
{
	INFO(PLUGINS, "Bluetooth plugin loaded.");
	Bluetooth* provider = new Bluetooth("bluetooth");
	provider->add(std::make_shared<ModeChange>());
	provider->add(std::make_shared<DesktopConnectivity>());
	provider->add(std::make_shared<Pairing>());
	provider->add(std::make_shared<Tethering>());

	return provider;
}
