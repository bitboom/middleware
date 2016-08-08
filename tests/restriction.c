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

#include <dpm/restriction.h>

#include "testbench.h"

#define STRINGIFY_(x) #x
#define STRINGIFY(x)  STRINGIFY_(x)

#define DEFINE_RESTRICTION_TESTCASE(__name__)                          \
static int restriction_##__name__(struct testcase *tc)                 \
{                                                                      \
	device_policy_manager_h handle;                                    \
	int state, enable = false;                                         \
	handle = dpm_manager_create();                                     \
	if (handle == NULL) {                                              \
		printf("Failed to create client handle\n");                    \
		return TEST_FAILED;                                            \
	}                                                                  \
	if (dpm_restriction_get_##__name__##_state(NULL, &state) == 0) {   \
		printf("NULL handle test in getter failed\n");                 \
		dpm_manager_destroy(handle);                                   \
		return TEST_FAILED;                                            \
	}                                                                  \
	if (dpm_restriction_get_##__name__##_state(handle, NULL) == 0) {   \
		printf("NULL param test failed\n");                            \
		dpm_manager_destroy(handle);                                   \
		return TEST_FAILED;                                            \
	}                                                                  \
	if (dpm_restriction_set_##__name__##_state(NULL, false) == 0) {    \
		printf("NULL handle test in setter failed\n");                 \
		dpm_manager_destroy(handle);                                   \
		return TEST_FAILED;                                            \
	}                                                                  \
	if (dpm_restriction_get_##__name__##_state(handle, &state) != 0) { \
		printf("Policy query failed\n");                               \
		dpm_manager_destroy(handle);                                   \
		return TEST_FAILED;                                            \
	}                                                                  \
	enable = !state;                                                   \
	if (dpm_restriction_set_##__name__##_state(handle, enable) != 0) { \
		printf("Policy enforce failed\n");                             \
		dpm_manager_destroy(handle);                                   \
		return TEST_FAILED;                                            \
	}                                                                  \
	dpm_restriction_get_##__name__##_state(handle, &enable);           \
	if (enable != !state) {                                            \
		printf("Policy check failed\n");                               \
		dpm_manager_destroy(handle);                                   \
		return TEST_FAILED;                                            \
	}                                                                  \
	if (dpm_restriction_set_##__name__##_state(handle, state) != 0) {  \
		printf("Policy recovery failed\n");                            \
		dpm_manager_destroy(handle);                                   \
		return TEST_FAILED;                                            \
	}                                                                  \
	dpm_manager_destroy(handle);                                       \
	return TEST_SUCCESSED;                                             \
}                                                                      \
struct testcase restriction_testcase_##__name__ = {                    \
	.description = STRINGIFY(__name__),                                \
	.handler = restriction_##__name__                                  \
}


static int restriction_messaging(struct testcase *tc)
{
	device_policy_manager_h handle;
	int state, enable = false;
	handle = dpm_manager_create();
	if (handle == NULL) {
		printf("Failed to create client handle\n");
		return TEST_FAILED;
	}
	if (dpm_restriction_get_messaging_state(NULL, "SIM1", &state) == 0) {
		printf("NULL handle test in getter failed\n");
		dpm_context_destroy(handle);
		return TEST_FAILED;
	}
	if (dpm_restriction_get_messaging_state(handle, "SIM1", NULL) == 0) {
		printf("NULL param test failed\n");
		dpm_context_destroy(handle);
		return TEST_FAILED;
	}
	if (dpm_restriction_set_messaging_state(NULL, "SIM1", false) == 0) {
		printf("NULL handle test in setter failed\n");
		dpm_context_destroy(handle);
		return TEST_FAILED;
	}
	if (dpm_restriction_get_messaging_state(handle, "SIM1", &state) != 0) {
		printf("Policy query failed\n");
		dpm_context_destroy(handle);
		return TEST_FAILED;
	}
	enable = !state;
	if (dpm_restriction_set_messaging_state(handle, "SIM1", enable) != 0) {
		printf("Policy enforce failed\n");
		dpm_context_destroy(handle);
		return TEST_FAILED;
	}
	dpm_restriction_get_messaging_state(handle, "SIM1", &enable);
	if (enable != !state) {
		printf("Policy check failed\n");
		dpm_context_destroy(handle);
		return TEST_FAILED;
	}
	if (dpm_restriction_set_messaging_state(handle, "SIM1", state) != 0) {
		printf("Policy recovery failed\n");
		dpm_context_destroy(handle);
		return TEST_FAILED;
	}
	dpm_context_destroy(handle);
	return TEST_SUCCESSED;
}
struct testcase restriction_testcase_messaging = {
	.description = "restriction_testcase_messaging",
	.handler = restriction_messaging
};

DEFINE_RESTRICTION_TESTCASE(clipboard);
DEFINE_RESTRICTION_TESTCASE(wifi);
DEFINE_RESTRICTION_TESTCASE(wifi_hotspot);
DEFINE_RESTRICTION_TESTCASE(bluetooth_tethering);
DEFINE_RESTRICTION_TESTCASE(usb_tethering);
DEFINE_RESTRICTION_TESTCASE(bluetooth_mode_change);
DEFINE_RESTRICTION_TESTCASE(bluetooth_desktop_connectivity);
DEFINE_RESTRICTION_TESTCASE(bluetooth_pairing);
DEFINE_RESTRICTION_TESTCASE(popimap_email);
DEFINE_RESTRICTION_TESTCASE(browser);
DEFINE_RESTRICTION_TESTCASE(camera);
DEFINE_RESTRICTION_TESTCASE(microphone);
DEFINE_RESTRICTION_TESTCASE(location);
DEFINE_RESTRICTION_TESTCASE(external_storage);
DEFINE_RESTRICTION_TESTCASE(usb_debugging);

void TESTCASE_CONSTRUCTOR restriction_policy_build_testcase(void)
{
	testbench_populate_testcase(&restriction_testcase_clipboard);
	testbench_populate_testcase(&restriction_testcase_usb_debugging);
	testbench_populate_testcase(&restriction_testcase_external_storage);
	testbench_populate_testcase(&restriction_testcase_location);
	testbench_populate_testcase(&restriction_testcase_microphone);
	testbench_populate_testcase(&restriction_testcase_camera);
	testbench_populate_testcase(&restriction_testcase_wifi);
	testbench_populate_testcase(&restriction_testcase_wifi_hotspot);
	testbench_populate_testcase(&restriction_testcase_bluetooth_tethering);
	testbench_populate_testcase(&restriction_testcase_usb_tethering);
	testbench_populate_testcase(&restriction_testcase_bluetooth_mode_change);
	testbench_populate_testcase(&restriction_testcase_bluetooth_desktop_connectivity);
	testbench_populate_testcase(&restriction_testcase_bluetooth_pairing);
	testbench_populate_testcase(&restriction_testcase_messaging);
	testbench_populate_testcase(&restriction_testcase_popimap_email);
	testbench_populate_testcase(&restriction_testcase_browser);
}
