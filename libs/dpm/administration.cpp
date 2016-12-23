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

#include <cstring>
#include <cassert>

#include "administration.h"
#include "administration.hxx"

#include "debug.h"
#include "policy-client.h"

using namespace DevicePolicyManager;

EXPORT_API int dpm_admin_register_client(device_policy_manager_h handle, const char* name, uid_t uid)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(name, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(uid, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	AdministrationPolicy admin = client.createPolicyInterface<AdministrationPolicy>();

	try {
		return admin.registerPolicyClient(name, uid);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_admin_deregister_client(device_policy_manager_h handle, const char* name, uid_t uid)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(name, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(uid, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	AdministrationPolicy admin = client.createPolicyInterface<AdministrationPolicy>();

	try {
		return admin.deregisterPolicyClient(name, uid);
	} catch (...) {
		return -1;
	}
}
