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

#include <system_info.h>
#include <klay/filesystem.h>

#include <klay/dbus/signal.h>
#include <klay/audit/logger.h>

#include "policy-client.h"
#include "policy-event-env.h"

namespace {

const std::string POLICY_MANAGER_ADDRESS = "/tmp/.device-policy-manager.sock";

int GetPolicyEnforceMode()
{
	runtime::File policyManagerSocket(POLICY_MANAGER_ADDRESS);

	if (policyManagerSocket.exists()) {
		return 1;
	}

	return 0;
}

} // namespace


DevicePolicyClient::DevicePolicyClient() noexcept :
	maintenanceMode(GetPolicyEnforceMode())
{
}

DevicePolicyClient::~DevicePolicyClient() noexcept
{
	disconnect();
}

int DevicePolicyClient::connect(const std::string& address) noexcept
{
	try {
		client.reset(new rmi::Client(address));
		if (maintenanceMode) {
			client->connect();
		}
	} catch (runtime::Exception& e) {
		return -1;
	}

	return 0;
}

int DevicePolicyClient::connect() noexcept
{
	return connect(POLICY_MANAGER_ADDRESS);
}

void DevicePolicyClient::disconnect() noexcept
{
	client.reset();
}

int DevicePolicyClient::subscribeSignal(const std::string& name,
										const SignalHandler& handler,
										void* data) noexcept
{
	if (!maintenanceMode)
		return 0;

	try {
		auto dispatch = [name, handler, data](dbus::Variant variant) {
			char *state = NULL;
			variant.get("(s)", &state);

			handler(name.c_str(), state, data);
		};

		dbus::signal::Receiver receiver(PIL_OBJECT_PATH, PIL_EVENT_INTERFACE);
		return receiver.subscribe(name, dispatch);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}
}

int DevicePolicyClient::unsubscribeSignal(int id) noexcept
{
	if (!maintenanceMode)
		return 0;

	try {
		dbus::signal::Receiver receiver(PIL_OBJECT_PATH, PIL_EVENT_INTERFACE);
		receiver.unsubscribe(id);
		return 0;
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}
}

DevicePolicyClient& GetDevicePolicyClient(void* handle)
{
	return *reinterpret_cast<DevicePolicyClient*>(handle);
}
