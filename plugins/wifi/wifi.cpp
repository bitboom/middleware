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

#include "wifi.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <klay/dbus/connection.h>

#include <memory>

#define NETCONFIG_INTERFACE		\
	"net.netconfig",			\
	"/net/netconfig/network",	\
	"net.netconfig.network"

namespace vist {
namespace policy {
namespace plugin {

void WifiState::onChanged(const PolicyValue& value)
{
	int enable = value;
	INFO(VIST_PLUGIN) << "Wifi state is changed to " << enable;
	klay::dbus::Connection &systemDBus = klay::dbus::Connection::getSystem();
	systemDBus.methodcall(NETCONFIG_INTERFACE,
						  "DevicePolicySetWifi",
						  -1,
						  "",
						  "(i)",
						  enable);
}

void ProfileChange::onChanged(const PolicyValue& value)
{
	int enable = value;
	INFO(VIST_PLUGIN) << "Wifi profile change state is changed to " << enable;
	dbus::Connection &systemDBus = dbus::Connection::getSystem();
	systemDBus.methodcall(NETCONFIG_INTERFACE,
						  "DevicePolicySetWifiProfile",
						  -1,
						  "",
						  "(i)",
						  enable);
}

void Hotspot::onChanged(const PolicyValue& value)
{
	int enable = value;
	INFO(VIST_PLUGIN) << "Wifi hotspot change state is changed to " << enable;
}

void SsidRestriction::onChanged(const PolicyValue& value)
{
	int enable = value;
	INFO(VIST_PLUGIN) << "Wifi ssid restriction change state is changed to " << enable;
}

Wifi::Wifi(const std::string& name) : PolicyProvider(name)
{
	int ret = ::wifi_manager_initialize(&handle);
	if (ret != WIFI_MANAGER_ERROR_NONE) {
		if (ret == WIFI_MANAGER_ERROR_NOT_SUPPORTED)
			ERROR("Wifi manager is not supported.");

		THROW(ErrCode::RuntimeError) << "Failed to init WiFi Manager.";
	}
}

Wifi::~Wifi()
{
	::wifi_manager_deinitialize(handle);
}

// TODO(Sangwan): Add privilege to provider
#define PRIVILEGE "http://tizen.org/privilege/dpm.wifi"

extern "C" PolicyProvider* PolicyFactory()
{
	INFO(VIST_PLUGIN) << "Wifi plugin loaded.";
	Wifi* provider = new Wifi("wifi");
	provider->add(std::make_shared<WifiState>());
	provider->add(std::make_shared<ProfileChange>());
	provider->add(std::make_shared<Hotspot>());
	provider->add(std::make_shared<SsidRestriction>());

	return provider;
}

} // namespace plugin
} // namespace policy
} // namespace vist
