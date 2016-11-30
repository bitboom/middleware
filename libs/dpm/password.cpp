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

#include "password.h"
#include "password_internal.h"
#include "password.hxx"

#include "array.h"
#include "debug.h"
#include "policy-client.h"

using namespace DevicePolicyManager;

EXPORT_API int dpm_password_set_quality(device_policy_manager_h handle, int quality)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setQuality(quality);
}

EXPORT_API int dpm_password_get_quality(device_policy_manager_h handle, int *quality)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(quality, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getQuality();
	if (ret < 0) {
		return -1;
	}

	*quality = (dpm_password_quality_e)ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_minimum_length(device_policy_manager_h handle, int value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setMinimumLength(value);
}

EXPORT_API int dpm_password_get_minimum_length(device_policy_manager_h handle, int *value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getMinimumLength();
	if (ret < 0) {
		return -1;
	}

	*value = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_min_complex_chars(device_policy_manager_h handle, int value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setMinComplexChars(value);
}

EXPORT_API int dpm_password_get_min_complex_chars(device_policy_manager_h handle, int *value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getMinComplexChars();
	if (ret < 0) {
		return -1;
	}

	*value = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_maximum_failed_attempts_for_wipe(device_policy_manager_h handle, int value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setMaximumFailedForWipe(value);
}

EXPORT_API int dpm_password_get_maximum_failed_attempts_for_wipe(device_policy_manager_h handle, int *value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getMaximumFailedForWipe();
	if (ret < 0) {
		return -1;
	}

	*value = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_expires(device_policy_manager_h handle, int value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setExpires(value);
}

EXPORT_API int dpm_password_get_expires(device_policy_manager_h handle, int *value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getExpires();
	if (ret < 0) {
		return -1;
	}

	*value = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_history(device_policy_manager_h handle, int value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setHistory(value);
}

EXPORT_API int dpm_password_get_history(device_policy_manager_h handle, int *value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getHistory();
	if (ret < 0) {
		return -1;
	}

	*value = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_pattern(device_policy_manager_h handle, const char *pattern)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(pattern, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setPattern(pattern);
}

EXPORT_API int dpm_password_reset(device_policy_manager_h handle, const char *passwd)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(passwd, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.reset(passwd);
}

EXPORT_API int dpm_password_enforce_change(device_policy_manager_h handle)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.enforceChange();
}

EXPORT_API int dpm_password_set_max_inactivity_time_device_lock(device_policy_manager_h handle, int value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setMaxInactivityTimeDeviceLock(value);
}

EXPORT_API int dpm_password_get_max_inactivity_time_device_lock(device_policy_manager_h handle, int *value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getMaxInactivityTimeDeviceLock();
	if (ret < 0) {
		return -1;
	}

	*value = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_status(device_policy_manager_h handle, dpm_password_status_e status)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(status >= DPM_PASSWORD_STATUS_NORMAL &&
				   status <= DPM_PASSWORD_STATUS_MAX_ATTEMPTS_EXCEEDED,
				   DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setStatus(status);
}

EXPORT_API int dpm_password_get_status(device_policy_manager_h handle, dpm_password_status_e *status)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(status, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getStatus();
	if (ret < 0) {
		return -1;
	}

	*status = (dpm_password_status_e)ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_delete_pattern(device_policy_manager_h handle)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.deletePattern();
}

EXPORT_API int dpm_password_get_pattern(device_policy_manager_h handle, char **pattern)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(pattern, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	*pattern = ::strdup(password.getPattern().c_str());

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_maximum_character_occurrences(device_policy_manager_h handle, int value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setMaximumCharacterOccurrences(value);
}

EXPORT_API int dpm_password_get_maximum_character_occurrences(device_policy_manager_h handle, int *value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getMaximumCharacterOccurrences();
	if (ret < 0) {
		return -1;
	}

	*value = ret;

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_maximum_numeric_sequence_length(device_policy_manager_h handle, int value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value >= 0, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setMaximumNumericSequenceLength(value);
}

EXPORT_API int dpm_password_get_maximum_numeric_sequence_length(device_policy_manager_h handle, int *value)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(value, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getMaximumNumericSequenceLength();
	if (ret < 0) {
		return -1;
	}

	*value = ret;

	return DPM_ERROR_NONE;
}

typedef runtime::Array<std::string> dpm_password_iterator;

EXPORT_API dpm_password_iterator_h dpm_password_create_iterator(device_policy_manager_h handle)
{
	RET_ON_FAILURE(handle, NULL);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return reinterpret_cast<dpm_password_iterator_h>(new dpm_password_iterator(password.getForbiddenStrings()));
}

EXPORT_API int dpm_password_iterator_next(dpm_password_iterator_h iter, const char **result)
{
	RET_ON_FAILURE(iter, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(result, DPM_ERROR_INVALID_PARAMETER);

	dpm_password_iterator *it = reinterpret_cast<dpm_password_iterator *>(iter);

	if (it->isEnd())
		*result = NULL;
	else
		*result = it->next()->c_str();

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_destroy_iterator(dpm_password_iterator_h iter)
{
	RET_ON_FAILURE(iter, DPM_ERROR_INVALID_PARAMETER);

	delete reinterpret_cast<dpm_password_iterator *>(iter);

	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_password_set_forbidden_strings(device_policy_manager_h handle, const char *strings[], int length)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(strings, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(length >= 0, DPM_ERROR_INVALID_PARAMETER);

	int iter;
	std::vector<std::string> forbiddenStrings;

	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(strings, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	for (iter = 0; iter < length; iter++)
		forbiddenStrings.push_back(strings[iter]);

	return password.setForbiddenStrings(forbiddenStrings);
}

EXPORT_API int dpm_password_set_recovery(device_policy_manager_h handle, int enable)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	return password.setRecovery(enable);
}

EXPORT_API int dpm_password_get_recovery(device_policy_manager_h handle, int *enable)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(enable, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	PasswordPolicy password = client.createPolicyInterface<PasswordPolicy>();

	int ret = password.getRecovery();
	if (ret < 0) {
		return -1;
	}

	*enable = ret;

	return DPM_ERROR_NONE;
}
