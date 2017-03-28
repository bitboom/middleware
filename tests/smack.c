/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <stdio.h>
#include <stdlib.h>
#include <dsm/smack.h>

#include "testbench.h"

static int smack_get_report(struct testcase *tc)
{
	int ret;
	device_policy_manager_h handle;

	handle = dpm_manager_create();
	if (handle == NULL) {
		printf("Failed to create client handle\n");
		return TEST_FAILED;
	}

	char *report = NULL;

	ret = TEST_SUCCESSED;
	if (dpm_smack_get_report(handle, &report) != DPM_ERROR_NONE) {
		ret = TEST_FAILED;
	}

	printf("Report: %s\n", report);

	dpm_smack_release_report(&report);

	dpm_manager_destroy(handle);

	return ret;
}

static int smack_get_issue_count(struct testcase *tc)
{
	int ret;
	device_policy_manager_h handle;

	handle = dpm_manager_create();
	if (handle == NULL) {
		printf("Failed to create client handle\n");
		return TEST_FAILED;
	}

	int count = -1;

	ret = TEST_SUCCESSED;
	if (dpm_smack_get_issue_count(handle, &count) != DPM_ERROR_NONE) {
		ret = TEST_FAILED;
	}

	if (count == -1) {
		ret = TEST_FAILED;
	}

	printf("Count: %d\n", count);

	dpm_manager_destroy(handle);

	return ret;
}

struct testcase smack_testcase_get_report = {
	.description = "smack_testcase_get_report",
	.handler = smack_get_report
};

struct testcase smack_testcase_get_issue_count = {
	.description = "smack_testcase_get_issue_count",
	.handler = smack_get_issue_count
};

void TESTCASE_CONSTRUCTOR smack_security_monitor_build_testcase(void)
{
	testbench_populate_testcase(&smack_testcase_get_report);
	testbench_populate_testcase(&smack_testcase_get_issue_count);
}
