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

#include "logger.h"

#include "../device-policy-manager.h"

#define RET_ON_FAILURE(cond, ret) \
{                                 \
	if (!(cond))                  \
		return (ret);             \
}

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
	maintenanceMode(GetPolicyEnforceMode()), clientAddress(POLICY_MANAGER_ADDRESS)
{
}

DevicePolicyClient::~DevicePolicyClient() noexcept
{
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
		ERROR(DPM, e.what());
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
		ERROR(DPM, e.what());
		return -1;
	}
}

DevicePolicyClient& GetDevicePolicyClient(void* handle)
{
	return *reinterpret_cast<DevicePolicyClient*>(handle);
}

EXPORT_API device_policy_manager_h dpm_manager_create(void)
{
	DevicePolicyClient* client = new(std::nothrow) DevicePolicyClient();
	if (client == nullptr) {
		return NULL;
	}

	return reinterpret_cast<device_policy_manager_h>(client);
}

EXPORT_API int dpm_manager_destroy(device_policy_manager_h handle)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	delete &GetDevicePolicyClient(handle);

	return 0;
}

EXPORT_API int dpm_add_policy_changed_cb(device_policy_manager_h handle,
										 const char* name,
										 dpm_policy_changed_cb callback,
										 void* user_data,
										 int* id)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(name, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(callback, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(id, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient& client = GetDevicePolicyClient(handle);
	int ret = client.subscribeSignal(name, callback, user_data);
	if (ret < 0) {
		return -1;
	}

	*id = ret;
	return 0;
}

EXPORT_API int dpm_remove_policy_changed_cb(device_policy_manager_h handle, int id)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(id >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient& client = GetDevicePolicyClient(handle);
	return client.unsubscribeSignal(id);
}

EXPORT_API int dpm_add_signal_cb(device_policy_manager_h handle, const char* signal,
								 dpm_signal_cb callback, void* user_data, int* id)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(signal, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(callback, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(id, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient& context = GetDevicePolicyClient(handle);
	int ret = context.subscribeSignal(signal, callback, user_data);
	if (ret < 0) {
		return -1;
	}

	*id = ret;
	return 0;
}

EXPORT_API int dpm_remove_signal_cb(device_policy_manager_h handle, int id)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(id >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyClient& context = GetDevicePolicyClient(handle);
	return context.unsubscribeSignal(id);
}
