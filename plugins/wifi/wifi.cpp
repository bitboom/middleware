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

#include <arpa/inet.h>

#include <cstdlib>
#include <functional>
#include <unordered_set>

#include <wifi-manager.h>

#include <klay/dbus/connection.h>

#include <policyd/pil/policy-context.h>
#include <policyd/pil/policy-model.h>
#include <policyd/pil/policy-storage.h>
#include <policyd/pil/policy-event.h>

#include "../dlog.h"

#define NETCONFIG_INTERFACE		\
	"net.netconfig",			\
	"/net/netconfig/network",	\
	"net.netconfig.network"

class ModeChange : public GlobalPolicy<DataSetInt> {
public:
	ModeChange() : GlobalPolicy("wifi")
	{
		PolicyEventNotifier::create("wifi");
	}

	bool apply(const DataType& value)
	{
		int enable = value;
		try {
			dbus::Connection &systemDBus = dbus::Connection::getSystem();
			systemDBus.methodcall(NETCONFIG_INTERFACE,
								  "DevicePolicySetWifi",
								  -1,
								  "",
								  "(i)",
								  enable);
		} catch (runtime::Exception& e) {
			ERROR(PLUGINS, "Failed to chaneg Wi-Fi state");
			return false;
		}

		PolicyEventNotifier::emit("wifi", enable ? "allowed" : "disallowed");
		return true;
	}
};

class ProfileChange : public GlobalPolicy<DataSetInt> {
public:
	ProfileChange() : GlobalPolicy("wifi-profile-change")
	{
		PolicyEventNotifier::create("wifi_profile_change");
	}

	bool apply(const DataType& value)
	{
		int enable = value;
		try {
			dbus::Connection &systemDBus = dbus::Connection::getSystem();
			systemDBus.methodcall(NETCONFIG_INTERFACE,
								  "DevicePolicySetWifiProfile",
								  -1,
								  "",
								  "(i)",
								  enable);
		} catch (runtime::Exception& e) {
			ERROR(PLUGINS, "Failed to set Wi-Fi profile change restriction");
			return false;
		}
		PolicyEventNotifier::emit("wifi_profile_change", enable ? "allowed" : "disallowed");
		return true;
	}
};

class Hotspot : public GlobalPolicy<DataSetInt> {
public:
	Hotspot() : GlobalPolicy("wifi-hotspot")
	{
		PolicyEventNotifier::create("wifi_hotspot");
	}

	bool apply(const DataType& value)
	{
		int enable = value;
		PolicyEventNotifier::emit("wifi_hotspot", enable ? "allowed" : "disallowed");
		return true;
	}
};

class Wifi : public AbstractPolicyProvider {
public:
	Wifi();
	~Wifi();

	int setState(bool enable);
	bool getState();
	int setHotspotState(bool enable);
	bool getHotspotState();
	int setProfileChangeRestriction(bool enable);
	bool isProfileChangeRestricted();

	static void onConnectionStateChanged(wifi_manager_connection_state_e state,
										 wifi_manager_ap_h ap, void *user_data);

private:
	wifi_manager_h handle;

	ModeChange modeChange;
	ProfileChange profileChange;
	Hotspot hotspot;
};


Wifi::Wifi() : handle(nullptr)
{
	int ret = 0;

	ret = ::wifi_manager_initialize(&handle);
	if (ret != WIFI_MANAGER_ERROR_NONE) {
		if (ret == WIFI_MANAGER_ERROR_NOT_SUPPORTED) {
			return;
		}
		throw runtime::Exception("WiFi Manager initialization failed");
	}

	ret = ::wifi_manager_set_connection_state_changed_cb(handle, &onConnectionStateChanged, this);
	if (ret != WIFI_MANAGER_ERROR_NONE) {
		throw runtime::Exception("WiFi Manager set connection state changed callback failed");
	}
}

Wifi::~Wifi()
{
	if (handle) {
		::wifi_manager_unset_connection_state_changed_cb(handle);
		::wifi_manager_deinitialize(handle);
	}
}

void Wifi::onConnectionStateChanged(wifi_manager_connection_state_e state,
									wifi_manager_ap_h ap, void *user_data)
{
	if (state == WIFI_MANAGER_CONNECTION_STATE_FAILURE ||
		state == WIFI_MANAGER_CONNECTION_STATE_DISCONNECTED) {
		return;
	}
}

int Wifi::setState(bool enable)
{
	try {
		modeChange.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, e.what());
		return -1;
	}

	return 0;
}

bool Wifi::getState()
{
	return modeChange.get();
}

int Wifi::setHotspotState(bool enable)
{
	try {
		hotspot.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, e.what());
		return -1;
	}

	return 0;
}

bool Wifi::getHotspotState()
{
	return hotspot.get();
}

int Wifi::setProfileChangeRestriction(bool enable)
{
	try {
		profileChange.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, e.what());
		return -1;
	}

	return 0;
}

bool Wifi::isProfileChangeRestricted()
{
	return profileChange.get();
}

extern "C" {

#define PRIVILEGE "http://tizen.org/privilege/dpm.wifi"

AbstractPolicyProvider *PolicyFactory(PolicyControlContext& context)
{
	INFO(PLUGINS, "Wifi plugin loaded");
	Wifi *policy = new Wifi();

	context.expose(policy, PRIVILEGE, (int)(Wifi::setState)(bool));
	context.expose(policy, PRIVILEGE, (int)(Wifi::setHotspotState)(bool));
	context.expose(policy, PRIVILEGE, (int)(Wifi::setProfileChangeRestriction)(bool));

	context.expose(policy, "", (bool)(Wifi::getState)());
	context.expose(policy, "", (bool)(Wifi::getHotspotState)());
	context.expose(policy, "", (bool)(Wifi::isProfileChangeRestricted)());

	return policy;
}

} // extern "C"
