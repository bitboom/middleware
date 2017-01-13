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

#include <cassert>
#include <iostream>

#include "device-policy-manager.h"

#include "policy-client.h"
#include "debug.h"

DevicePolicyContext& GetDevicePolicyContext(void* handle)
{
	assert(handle);
	return *reinterpret_cast<DevicePolicyContext*>(handle);
}

EXPORT_API device_policy_manager_h dpm_manager_create(void)
{
	DevicePolicyContext* client = new(std::nothrow) DevicePolicyContext();
	if (client == nullptr) {
		return NULL;
	}

	if (client->connect() < 0) {
		delete client;
		return NULL;
	}

	return reinterpret_cast<device_policy_manager_h>(client);
}

EXPORT_API int dpm_manager_destroy(device_policy_manager_h handle)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	delete &GetDevicePolicyContext(handle);

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

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	int ret = client.subscribePolicyChange(name, callback, user_data);
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

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	return client.unsubscribePolicyChange(id);
}

EXPORT_API int dpm_add_signal_cb(device_policy_manager_h handle, const char* signal,
								 dpm_signal_cb callback, void* user_data, int* id)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(signal, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(callback, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(id, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext& context = GetDevicePolicyContext(handle);
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

	DevicePolicyContext& context = GetDevicePolicyContext(handle);
	return context.unsubscribeSignal(id);
}
