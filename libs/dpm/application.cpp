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

#include "application.h"
#include "application.hxx"

#include "debug.h"
#include "policy-client.h"

using namespace DevicePolicyManager;

EXPORT_API int dpm_application_set_mode_restriction(device_policy_manager_h handle, int mode)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(mode, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	ApplicationPolicy application = client.createPolicyInterface<ApplicationPolicy>();

	try {
		return application.setModeRestriction(mode);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_application_unset_mode_restriction(device_policy_manager_h handle, int mode)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(mode, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	ApplicationPolicy application = client.createPolicyInterface<ApplicationPolicy>();

	try {
		return application.unsetModeRestriction(mode);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_application_get_mode_restriction(device_policy_manager_h handle, int *mode)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(mode, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	ApplicationPolicy application = client.createPolicyInterface<ApplicationPolicy>();

	try {
		int ret = application.getModeRestriction();
		if (ret < 0) {
			return -1;
		}
		*mode = ret;
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_application_add_privilege_to_blacklist(device_policy_manager_h handle, int type, const char* privilege)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(privilege, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	ApplicationPolicy application = client.createPolicyInterface<ApplicationPolicy>();

	try {
		return application.addPrivilegeToBlacklist(type, privilege);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_application_remove_privilege_from_blacklist(device_policy_manager_h handle, int type, const char* privilege)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(privilege, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	ApplicationPolicy application = client.createPolicyInterface<ApplicationPolicy>();

	try {
		return application.removePrivilegeFromBlacklist(type, privilege);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_application_check_privilege_is_blacklisted(device_policy_manager_h handle, int type, const char* privilege, int *is_blacklisted)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(privilege, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(is_blacklisted, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	ApplicationPolicy application = client.createPolicyInterface<ApplicationPolicy>();

	try {
		int ret = application.checkPrivilegeIsBlacklisted(type, privilege);
		if (ret < 0) {
			return -1;
		}

		*is_blacklisted = ret;
	} catch (...) {
		return -1;
	}

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_application_install_package(device_policy_manager_h handle, const char* pkgpath)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(pkgpath, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	ApplicationPolicy application = client.createPolicyInterface<ApplicationPolicy>();

	try {
		return application.installPackage(pkgpath);
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_application_uninstall_package(device_policy_manager_h handle, const char* pkgid)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(pkgid, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext& client = GetDevicePolicyContext(handle);
	ApplicationPolicy application = client.createPolicyInterface<ApplicationPolicy>();

	try {
		return application.uninstallPackage(pkgid);
	} catch (...) {
		return -1;
	}
}
