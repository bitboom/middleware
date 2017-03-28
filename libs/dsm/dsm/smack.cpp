/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

/**
 *  @file smack.cpp
 *  @brief Implementation of SMACK agent module.
 *  @author Dmytro Logachev (d.logachev@samsung.com)
 *  @date Created Oct 13, 2016
 *  @par In Samsung Ukraine R&D Center (SRK) under a contract between
 *  @par LLC "Samsung Electronics Ukraine Company" (Kyiv, Ukraine)
 *  @par and "Samsung Electronics Co", Ltd (Seoul, Republic of Korea)
 *  @par Copyright: (c) Samsung Electronics Co, Ltd 2016. All rights reserved.
**/

#include <cstring>
#include <cassert>

#include "smack.h"
#include "smack.hxx"

#include "debug.h"
#include "policy-client.h"

using namespace DevicePolicyManager;

EXPORT_API int dpm_smack_get_report(device_policy_manager_h handle, char **report)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(report, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	SmackSecurityMonitor smack = client.createPolicyInterface<SmackSecurityMonitor>();

	try {
		std::string str = smack.getReport();
		if (!str.empty()) {
			if (str.compare("[]") == 0) {
				return DPM_ERROR_INVALID_PARAMETER;
			}
			*report = ::strdup(str.c_str());
			if (*report == 0)
				return DPM_ERROR_OUT_OF_MEMORY;
		} else {
			return DPM_ERROR_TIMED_OUT;
		}

		return DPM_ERROR_NONE;
	} catch (...) {
		return -1;
	}
}

EXPORT_API int dpm_smack_release_report(char **report)
{
	RET_ON_FAILURE(*report, DPM_ERROR_INVALID_PARAMETER);

	free(*report);
	*report = nullptr;
	return DPM_ERROR_NONE;
}

EXPORT_API int dpm_smack_get_issue_count(device_policy_manager_h handle, int *count)
{
	RET_ON_FAILURE(handle, DPM_ERROR_INVALID_PARAMETER);
	RET_ON_FAILURE(count, DPM_ERROR_INVALID_PARAMETER);

	DevicePolicyContext &client = GetDevicePolicyContext(handle);
	SmackSecurityMonitor smack = client.createPolicyInterface<SmackSecurityMonitor>();

	try {
		int issueCount = smack.getIssueCount();
		if (issueCount == -1) {
			return DPM_ERROR_OUT_OF_MEMORY;
		}

		*count = issueCount;

		return DPM_ERROR_NONE;
	} catch (...) {
		return -1;
	}
}
