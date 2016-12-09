/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <cstdlib>
#include <functional>
#include <unordered_set>

#include <glib.h>
#include <vconf.h>
#include <wifi.h>
#include <vconf-keys.h>
#include <arpa/inet.h>
#include <klay/audit/logger.h>
#include <klay/dbus/connection.h>

#include "privilege.h"
#include "app-bundle.h"
#include "policy-builder.h"

#include "wifi.hxx"

#define NETCONFIG_INTERFACE		\
	"net.netconfig",			\
	"/net/netconfig/network",	\
	"net.netconfig.network"

namespace DevicePolicyManager {

namespace {

std::atomic<bool> blockEnabled;
std::unordered_set<std::string> blockSsidList;

inline void applyBlocklist(wifi_ap_h ap)
{
	char *ssid;
	::wifi_ap_get_essid(ap, &ssid);
	if (blockSsidList.find(ssid) != blockSsidList.end()) {
		::wifi_forget_ap(ap);
	}

	::free(ssid);
}

void connectionStateChanged(wifi_connection_state_e state,
								   wifi_ap_h ap, void *user_data)
{
	WifiPolicy* policy = reinterpret_cast<WifiPolicy*>(user_data);

	if (state == WIFI_CONNECTION_STATE_FAILURE ||
		state == WIFI_CONNECTION_STATE_DISCONNECTED) {
		return;
	}

	if (!policy->isNetworkAccessRestricted()) {
		return;
	}

	applyBlocklist(ap);
}

inline void applyBlocklistToConnectedAP()
{
	wifi_ap_h ap;

	::wifi_initialize();
	if (::wifi_get_connected_ap(&ap) == WIFI_ERROR_NONE) {
		applyBlocklist(ap);
		::wifi_ap_destroy(ap);
	}
}

std::vector<std::string> wifiNotifications = {
	"wifi",
	"wifi-hotspot",
	"wifi-profile-change",
	"wifi-ssid-restriction"
};

} // namespace

WifiPolicy::WifiPolicy(PolicyControlContext& ctx) :
	context(ctx)
{
	context.expose(this, DPM_PRIVILEGE_WIFI, (int)(WifiPolicy::setState)(bool));
	context.expose(this, DPM_PRIVILEGE_WIFI, (int)(WifiPolicy::setHotspotState)(bool));
	context.expose(this, DPM_PRIVILEGE_WIFI, (int)(WifiPolicy::setProfileChangeRestriction)(bool));
	context.expose(this, DPM_PRIVILEGE_WIFI, (int)(WifiPolicy::setNetworkAccessRestriction)(bool));
	context.expose(this, DPM_PRIVILEGE_WIFI, (int)(WifiPolicy::addSsidToBlocklist)(std::string));

	context.expose(this, "", (bool)(WifiPolicy::getState)());
	context.expose(this, "", (bool)(WifiPolicy::getHotspotState)());
	context.expose(this, "", (bool)(WifiPolicy::isProfileChangeRestricted)());
	context.expose(this, "", (bool)(WifiPolicy::isNetworkAccessRestricted)());
	context.expose(this, "", (int)(WifiPolicy::removeSsidFromBlocklist)(std::string));

	context.createNotification(wifiNotifications);

	::wifi_initialize();
	::wifi_set_connection_state_changed_cb(connectionStateChanged, this);
}

WifiPolicy::~WifiPolicy()
{
	::wifi_unset_connection_state_changed_cb();
}

int WifiPolicy::setState(bool enable)
{
	try {
		if (!SetPolicyAllowed(context, "wifi", enable)) {
			return 0;
		}

		dbus::Connection &systemDBus = dbus::Connection::getSystem();
		systemDBus.methodcall(NETCONFIG_INTERFACE,
							  "DevicePolicySetWifi",
							  -1,
							  "",
							  "(i)",
							  enable);
	} catch (runtime::Exception& e) {
		ERROR("Failed to chaneg Wi-Fi state");
		return -1;
	}

	return 0;
}

bool WifiPolicy::getState()
{
	return context.getPolicy<int>("wifi");
}

int WifiPolicy::setHotspotState(bool enable)
{
	try {
		if (!SetPolicyAllowed(context, "wifi-hotspot", enable)) {
			return 0;
		}
	} catch (runtime::Exception& e) {
		ERROR("Failed to change Wi-Fi hotspot state");
		return -1;
	}

	return 0;
}

bool WifiPolicy::getHotspotState()
{
	return context.getPolicy<int>("wifi-hotspot");
}

int WifiPolicy::setProfileChangeRestriction(bool enable)
{
	try {
		if (!SetPolicyAllowed(context, "wifi-profile-change", enable)) {
			return 0;
		}

		dbus::Connection &systemDBus = dbus::Connection::getSystem();
		systemDBus.methodcall(NETCONFIG_INTERFACE,
							  "DevicePolicySetWifiProfile",
							  -1,
							  "",
							  "(i)",
							  enable);
	} catch (runtime::Exception& e) {
		ERROR("Failed to set Wi-Fi profile change restriction");
	}

	return 0;
}

bool WifiPolicy::isProfileChangeRestricted(void)
{
	return context.getPolicy<int>("wifi-profile-change");
}

int WifiPolicy::setNetworkAccessRestriction(bool enable)
{
	try {
			if (!SetPolicyEnabled(context, "wifi-ssid-restriction", enable)) {
				return 0;
			}

			if (enable) {
				applyBlocklistToConnectedAP();
			}
	} catch (runtime::Exception& e) {
		ERROR("Failed to set Wi-Fi profile change restriction");
	}

	return 0;
}

bool WifiPolicy::isNetworkAccessRestricted(void)
{
	return context.getPolicy<int>("wifi-ssid-restriction");
}

int WifiPolicy::addSsidToBlocklist(const std::string& ssid)
{
	try {
		blockSsidList.insert(ssid);
		if (context.getPolicy<int>("wifi-ssid-restriction")) {
			applyBlocklistToConnectedAP();
		}
	} catch (runtime::Exception& e) {
		ERROR("Failed to allocate memory for wifi blocklist");
		return -1;
	}

	return 0;
}

int WifiPolicy::removeSsidFromBlocklist(const std::string& ssid)
{
	std::unordered_set<std::string>::iterator it = blockSsidList.find(ssid);

	if (it == blockSsidList.end()) {
		return -1;
	}

	blockSsidList.erase(it);
	return 0;
}

DEFINE_POLICY(WifiPolicy);

} // namespace DevicePolicyManager
