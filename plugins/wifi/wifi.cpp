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
#include <wifi-manager.h>

#include <vist/policy/sdk/global-policy.h>
#include <vist/policy/sdk/policy-provider.h>

#include <klay/dbus/connection.h>

#include <memory>

#include "../dlog.h"

#define NETCONFIG_INTERFACE		\
	"net.netconfig",			\
	"/net/netconfig/network",	\
	"net.netconfig.network"

using namespace vist::policy;

class ModeChange : public GlobalPolicy {
public:
	ModeChange() : GlobalPolicy("wifi", PolicyValue(1)) {}

	void onChanged(const PolicyValue& value) override
	{
		int enable = value;
		klay::dbus::Connection &systemDBus = klay::dbus::Connection::getSystem();
		systemDBus.methodcall(NETCONFIG_INTERFACE,
							  "DevicePolicySetWifi",
							  -1,
							  "",
							  "(i)",
							  enable);
	}
};

class ProfileChange : public GlobalPolicy {
public:
	ProfileChange() : GlobalPolicy("wifi-profile-change", PolicyValue(1)) {}

	void onChanged(const PolicyValue& value) override
	{
		int enable = value;
		dbus::Connection &systemDBus = dbus::Connection::getSystem();
		systemDBus.methodcall(NETCONFIG_INTERFACE,
							  "DevicePolicySetWifiProfile",
							  -1,
							  "",
							  "(i)",
							  enable);
	}
};

class Hotspot : public GlobalPolicy {
public:
	Hotspot() : GlobalPolicy("wifi-hotspot", PolicyValue(1)) {}

	void onChanged(const PolicyValue&) override
	{
		/// N/A
	}
};

class SsidRestriction : public GlobalPolicy {
public:
	SsidRestriction() : GlobalPolicy("wifi-ssid-restriction", PolicyValue(0)) {}

	void onChanged(const PolicyValue&) override
	{
		/// N/A
	}
};

class Wifi : public PolicyProvider {
public:
	Wifi(const std::string& name) : PolicyProvider(name)
	{
		int ret = ::wifi_manager_initialize(&handle);
		if (ret != WIFI_MANAGER_ERROR_NONE) {
			if (ret == WIFI_MANAGER_ERROR_NOT_SUPPORTED)
				return;

			throw std::runtime_error("WiFi Manager initialization failed.");
		}
	}

	~Wifi()
	{
		if (handle == nullptr)
			return;

		::wifi_manager_deinitialize(handle);
	}

private:
	::wifi_manager_h handle = nullptr;
};

// TODO(Sangwan): Add privilege to provider
#define PRIVILEGE "http://tizen.org/privilege/dpm.wifi"

extern "C" PolicyProvider* PolicyFactory()
{
	INFO(PLUGINS, "Wifi plugin loaded.");
	Wifi* provider = new Wifi("wifi");
	provider->add(std::make_shared<ModeChange>());
	provider->add(std::make_shared<ProfileChange>());
	provider->add(std::make_shared<Hotspot>());
	provider->add(std::make_shared<SsidRestriction>());

	return provider;
}
