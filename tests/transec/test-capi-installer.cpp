/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        test-capi-installer.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Unit test program of Trust Anchor CAPI for installer
 */

#include <dpl/test/test_runner.h>

#include "tanchor/trust-anchor.h"

#include "test-resource.h"

RUNNER_TEST_GROUP_INIT(T0800_CAPI_TRUST_ANCHOR_INSTALLER)

RUNNER_TEST(T0801_CAPI_TRUST_ANCHOR_INSTALL_GLOBAL_APP_POSITIVE)
{
	int ret = trust_anchor_global_install(DUMMY_PKG_ID, APP_CERTS_DIR, false);
	RUNNER_ASSERT_MSG(ret == 0, "Trust Anchor install should be success.");
}

RUNNER_TEST(T0802_CAPI_TRUST_ANCHOR_INSTALL_GLOBAL_APP_WITH_SYS_POSITIVE)
{
	int ret = trust_anchor_global_install(DUMMY_PKG_ID, APP_CERTS_DIR, true);
	RUNNER_ASSERT_MSG(ret == 0, "Trust Anchor install should be success.");
}

RUNNER_TEST(T0803_CAPI_TRUST_ANCHOR_INSTALL_USER_APP_POSITIVE)
{
	int ret = trust_anchor_usr_install(DUMMY_PKG_ID, APP_CERTS_DIR, DUMMY_UID, false);
	RUNNER_ASSERT_MSG(ret == 0, "Trust Anchor install should be success.");
}

RUNNER_TEST(T0804_CAPI_TRUST_ANCHOR_INSTALL_USER_APP_WITH_SYS_POSITIVE)
{
	int ret = trust_anchor_usr_install(DUMMY_PKG_ID, APP_CERTS_DIR, DUMMY_UID, true);
	RUNNER_ASSERT_MSG(ret == 0, "Trust Anchor install should be success.");
}

RUNNER_TEST(T0805_CAPI_TRUST_ANCHOR_INSTALL_GLOBAL_APP_NEGATIVE)
{
	int ret = trust_anchor_global_install(DUMMY_PKG_ID, DUMMY_CERTS_DIR, false);
	RUNNER_ASSERT_MSG(ret != 0, "Trust Anchor install should be fail.");
}

RUNNER_TEST(T0806_CAPI_TRUST_ANCHOR_INSTALL_GLOBAL_APP_WITH_SYS_NEGATIVE)
{
	int ret = trust_anchor_global_install(DUMMY_PKG_ID, DUMMY_CERTS_DIR, true);
	RUNNER_ASSERT_MSG(ret != 0, "Trust Anchor install should be fail.");
}

RUNNER_TEST(T0807_CAPI_TRUST_ANCHOR_INSTALL_USER_APP_NEGATIVE)
{
	int ret = trust_anchor_usr_install(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID, false);
	RUNNER_ASSERT_MSG(ret != 0, "Trust Anchor install should be fail.");
}

RUNNER_TEST(T0808_CAPI_TRUST_ANCHOR_INSTALL_USER_APP_WITH_SYS_NEGATIVE)
{
	int ret = trust_anchor_usr_install(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID, true);
	RUNNER_ASSERT_MSG(ret != 0, "Trust Anchor install should be fail.");
}

RUNNER_TEST(T0821_CAPI_TRUST_ANCHOR_UNINSTALL_GLOBAL_APP_POSITIVE)
{
	int ret = trust_anchor_global_install(DUMMY_PKG_ID, APP_CERTS_DIR, false);
	RUNNER_ASSERT_MSG(ret == 0, "Trust Anchor install should be success.");

	ret = trust_anchor_global_uninstall(DUMMY_PKG_ID, APP_CERTS_DIR);
	RUNNER_ASSERT_MSG(ret == 0, "Trust Anchor uninstall should be success.");
}

RUNNER_TEST(T0822_CAPI_TRUST_ANCHOR_UNINSTALL_USER_APP_POSITIVE)
{
	int ret = trust_anchor_usr_install(DUMMY_PKG_ID, APP_CERTS_DIR, DUMMY_UID, false);
	RUNNER_ASSERT_MSG(ret == 0, "Trust Anchor install should be success.");

	ret = trust_anchor_usr_uninstall(DUMMY_PKG_ID, APP_CERTS_DIR, DUMMY_UID);
	RUNNER_ASSERT_MSG(ret == 0, "Trust Anchor uninstall should be success.");
}

RUNNER_TEST(T0823_CAPI_TRUST_ANCHOR_UNINSTALL_GLOBAL_APP_NEGATIVE)
{
	int ret = trust_anchor_global_uninstall(DUMMY_PKG_ID, DUMMY_CERTS_DIR);
	RUNNER_ASSERT_MSG(ret != 0, "Trust Anchor uninstall should be fail.");
}

RUNNER_TEST(T0824_CAPI_TRUST_ANCHOR_UNINSTALL_USER_APP_NEGATIVE)
{
	int ret = trust_anchor_usr_uninstall(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID);
	RUNNER_ASSERT_MSG(ret != 0, "Trust Anchor uninstall should be fail.");
}
