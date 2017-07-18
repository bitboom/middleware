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

#include <arpa/inet.h>

#include <cstdlib>
#include <functional>
#include <unordered_set>

#include <wifi-manager.h>

#include <klay/dbus/connection.h>

#include "logger.h"
#include "privilege.h"
#include "app-bundle.h"
#include "policy-builder.h"
#include "policy-model.h"

#include "wifi.hxx"

#define NETCONFIG_INTERFACE		\
	"net.netconfig",			\
	"/net/netconfig/network",	\
	"net.netconfig.network"

namespace DevicePolicyManager {

class WifiStateEnforceModel : public BaseEnforceModel {
public:
	WifiStateEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool enable)
	{
		try {
			dbus::Connection &systemDBus = dbus::Connection::getSystem();
			systemDBus.methodcall(NETCONFIG_INTERFACE,
								  "DevicePolicySetWifi",
								  -1,
								  "",
								  "(i)",
								  enable);
		} catch (runtime::Exception& e) {
			ERROR(SINK, "Failed to chaneg Wi-Fi state");
			return false;
		}

		notify(enable == 0 ? "disallowed" : "allowed");
		return true;
	}
};

class WifiProfileChangeEnforceModel : public BaseEnforceModel {
public:
	WifiProfileChangeEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool enable)
	{
		try {
			dbus::Connection &systemDBus = dbus::Connection::getSystem();
			systemDBus.methodcall(NETCONFIG_INTERFACE,
								  "DevicePolicySetWifiProfile",
								  -1,
								  "",
								  "(i)",
								  enable);
		} catch (runtime::Exception& e) {
			ERROR(SINK, "Failed to set Wi-Fi profile change restriction");
			return false;
		}
		notify(enable == 0 ? "disallowed" : "allowed");
		return true;
	}
};

typedef GlobalPolicy<int, WifiStateEnforceModel> WifiStatePolicy;
typedef GlobalPolicy<int, WifiProfileChangeEnforceModel> WifiProfileChangePolicy;
typedef GlobalPolicy<int> WifiHotspotPolicy;

struct WifiPolicy::Private : public PolicyHelper {
	Private(PolicyControlContext& ctxt);
	~Private();

	static void onConnectionStateChanged(wifi_manager_connection_state_e state,
										 wifi_manager_ap_h ap, void *user_data);

	WifiStatePolicy wifiState{context, "wifi"};
	WifiHotspotPolicy wifiHotspot{context, "wifi-hotspot"};
	WifiProfileChangePolicy wifiProfileChange{context, "wifi-profile-change"};
	wifi_manager_h wifiHandle;
};

WifiPolicy::Private::Private(PolicyControlContext& ctxt) :
	PolicyHelper(ctxt), wifiHandle(nullptr)
{
	int ret = 0;

	ret = ::wifi_manager_initialize(&wifiHandle);
	if (ret != WIFI_MANAGER_ERROR_NONE) {
		if (ret == WIFI_MANAGER_ERROR_NOT_SUPPORTED) {
			return;
		}
		throw runtime::Exception("WiFi Manager initialization failed");
	}

	ret = ::wifi_manager_set_connection_state_changed_cb(wifiHandle, &onConnectionStateChanged, this);
	if (ret != WIFI_MANAGER_ERROR_NONE) {
		throw runtime::Exception("WiFi Manager set connection state changed callback failed");
	}
}

WifiPolicy::Private::~Private()
{
	if (wifiHandle) {
		::wifi_manager_unset_connection_state_changed_cb(wifiHandle);
		::wifi_manager_deinitialize(wifiHandle);
	}
}

void WifiPolicy::Private::onConnectionStateChanged(wifi_manager_connection_state_e state,
												 wifi_manager_ap_h ap, void *user_data)
{
	if (state == WIFI_MANAGER_CONNECTION_STATE_FAILURE ||
		state == WIFI_MANAGER_CONNECTION_STATE_DISCONNECTED) {
		return;
	}
}

WifiPolicy::WifiPolicy(WifiPolicy&& rhs) = default;
WifiPolicy& WifiPolicy::operator=(WifiPolicy&& rhs) = default;

WifiPolicy::WifiPolicy(const WifiPolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}


WifiPolicy& WifiPolicy::operator=(const WifiPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

WifiPolicy::WifiPolicy(PolicyControlContext& context) :
	pimpl(new Private(context))
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
}

WifiPolicy::~WifiPolicy()
{
}

int WifiPolicy::setState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->wifiState, enable);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

bool WifiPolicy::getState()
{
	return pimpl->getPolicy(pimpl->wifiState);
}

int WifiPolicy::setHotspotState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->wifiHotspot, enable);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

bool WifiPolicy::getHotspotState()
{
	return pimpl->getPolicy(pimpl->wifiHotspot);
}

int WifiPolicy::setProfileChangeRestriction(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->wifiProfileChange, enable);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

bool WifiPolicy::isProfileChangeRestricted()
{
	return pimpl->getPolicy(pimpl->wifiProfileChange);
}

int WifiPolicy::setNetworkAccessRestriction(bool enable)
{
	return -1;
}

bool WifiPolicy::isNetworkAccessRestricted()
{
	return false;
}

int WifiPolicy::addSsidToBlocklist(const std::string& ssid)
{
	return -1;
}

int WifiPolicy::removeSsidFromBlocklist(const std::string& ssid)
{
	return -1;
}

DEFINE_POLICY(WifiPolicy);

} // namespace DevicePolicyManager
