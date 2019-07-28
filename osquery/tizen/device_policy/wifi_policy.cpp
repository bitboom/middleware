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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or ManagerImplied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */

#include "wifi_policy.h"

/// TODO: Resolve macro ERROR conflicts.
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <osquery/logger.h>

#include <dpm/pil/policy-event.h>
#include <klay/dbus/connection.h>

namespace {

const std::string NETCONFIG_BUSNAME = "net.netconfig";
const std::string NETCONFIG_OBJECT = "net/netconfig/network";
const std::string NETCONFIG_INTERFACE = "net.netconfig.network";

} // anonymous namespace

namespace osquery {

const std::string WifiPolicy::PRIVILEGE = "http://tizen.org/privilege/dpm.wifi";

WifiPolicy::Wifi::Wifi() : GlobalPolicy("wifi")
{
	PolicyEventNotifier::create("wifi");
}

bool WifiPolicy::Wifi::apply(const DataType& value) try
{
	int enable = value;
	klay::dbus::Connection &systemDBus = klay::dbus::Connection::getSystem();
	systemDBus.methodcall(NETCONFIG_BUSNAME,
						  NETCONFIG_OBJECT,
						  NETCONFIG_INTERFACE,
						  "DevicePolicySetWifi",
						  -1,
						  "",
						  "(i)",
						  enable);
	PolicyEventNotifier::emit("wifi", enable ? "allowed" : "disallowed");
	return true;
} catch (runtime::Exception& e)
{
	VLOG(1) << "Failed to change Wi-Fi state";
	return false;
}

WifiPolicy::Profile::Profile() : GlobalPolicy("wifi-profile-change")
{
	PolicyEventNotifier::create("wifi_profile_change");
}

bool WifiPolicy::Profile::apply(const DataType& value) try
{
	int enable = value;
	klay::dbus::Connection &systemDBus = klay::dbus::Connection::getSystem();
	systemDBus.methodcall(NETCONFIG_BUSNAME,
						  NETCONFIG_OBJECT,
						  NETCONFIG_INTERFACE,
						  "DevicePolicySetWifiProfile",
						  -1,
						  "",
						  "(i)",
						  enable);
	PolicyEventNotifier::emit("wifi_profile_change", enable ? "allowed" : "disallowed");
	return true;
} catch (runtime::Exception& e)
{
	VLOG(1) << "Failed to change Profile state";
	return false;
}

WifiPolicy::Hotspot::Hotspot() : GlobalPolicy("wifi-hotspot")
{
	PolicyEventNotifier::create("wifi_hotspot");
}

bool WifiPolicy::Hotspot::apply(const DataType& value) try
{
	int enable = value;
	PolicyEventNotifier::emit("wifi_hotspot", enable ? "allowed" : "disallowed");
	return true;
} catch (runtime::Exception& e)
{
	VLOG(1) << "Failed to change Hotspot state";
	return false;
}

WifiPolicy::WifiPolicy()
{
	int ret = ::wifi_manager_initialize(&handle);
	if (ret != WIFI_MANAGER_ERROR_NONE) {
		if (ret == WIFI_MANAGER_ERROR_NOT_SUPPORTED)
			throw std::runtime_error("WiFi Manager isn't supported.");

		throw std::runtime_error("WiFi Manager initialization failed");
	}

	ret = ::wifi_manager_set_connection_state_changed_cb(handle, &onConnection, nullptr);
	if (ret != WIFI_MANAGER_ERROR_NONE)
		VLOG(1) << "WiFi Manager set connection state changed callback failed";
}

WifiPolicy::~WifiPolicy()
{
	::wifi_manager_unset_connection_state_changed_cb(handle);
	::wifi_manager_deinitialize(handle);
}

void WifiPolicy::onConnection(wifi_manager_connection_state_e state,
							  wifi_manager_ap_h ap,
							  void *user_data)
{
	/// TODO: This section is able to check policy violation.
	if (state == WIFI_MANAGER_CONNECTION_STATE_FAILURE ||
		state == WIFI_MANAGER_CONNECTION_STATE_DISCONNECTED)
		return;
}

void WifiPolicy::setWifi(bool enable)
{
	wifi.set(enable);
}

bool WifiPolicy::getWifi()
{
	return wifi.get();
}

void WifiPolicy::setProfile(bool enable)
{
	profile.set(enable);
}

bool WifiPolicy::getProfile()
{
	return profile.get();
}

void WifiPolicy::setHotspot(bool enable)
{
	hotspot.set(enable);
}

bool WifiPolicy::getHotspot()
{
	return hotspot.get();
}

} // namespace osquery
