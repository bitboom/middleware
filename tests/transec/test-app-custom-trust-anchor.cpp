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
 * @file        test-app-custom-trust-anchor.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Unit test program of AppCustomTrustAnchor
 */

#include <dpl/test/test_runner.h>

#include <AppCustomTrustAnchor.h>

#include "test-resource.h"

RUNNER_TEST_GROUP_INIT(T0400_API_ACTA)

using namespace transec;

RUNNER_TEST(T0401_ACTA_INSTALL_GLOBAL_APP_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, APP_CERTS_DIR);
	int ret = acta.install(false);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA install should be success.");
}

RUNNER_TEST(T0402_ACTA_INSTALL_GLOBAL_APP_WITH_SYS_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, APP_CERTS_DIR);
	int ret = acta.install(true);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA install should be success.");
}

RUNNER_TEST(T0403_ACTA_INSTALL_USER_APP_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, APP_CERTS_DIR, DUMMY_UID);
	int ret = acta.install(false);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA install should be success.");
}

RUNNER_TEST(T0404_ACTA_INSTALL_USER_APP_WITH_SYS_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, APP_CERTS_DIR, DUMMY_UID);
	int ret = acta.install(true);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA install should be success.");
}

RUNNER_TEST(T0405_ACTA_INSTALL_GLOBAL_APP_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR);
	int ret = acta.install(false);
	RUNNER_ASSERT_MSG(ret != 0, "ACTA install should be fail.");
}

RUNNER_TEST(T0406_ACTA_INSTALL_GLOBAL_APP_WITH_SYS_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR);
	int ret = acta.install(true);
	RUNNER_ASSERT_MSG(ret != 0, "ACTA install should be fail.");
}

RUNNER_TEST(T0407_ACTA_INSTALL_USER_APP_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID);
	int ret = acta.install(false);
	RUNNER_ASSERT_MSG(ret != 0, "ACTA install should be fail.");
}

RUNNER_TEST(T0408_ACTA_INSTALL_USER_APP_WITH_SYS_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID);
	int ret = acta.install(true);
	RUNNER_ASSERT_MSG(ret != 0, "ACTA install should be fail.");
}

RUNNER_TEST(T0421_ACTA_UNINSTALL_GLOBAL_APP_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, APP_CERTS_DIR);
	int ret = acta.install(false);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA install should be success.");

	ret = acta.uninstall();
	RUNNER_ASSERT_MSG(ret == 0, "ACTA uninstall should be success.");
}

RUNNER_TEST(T0422_ACTA_UNINSTALL_USER_APP_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, APP_CERTS_DIR, DUMMY_UID);
	int ret = acta.install(false);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA install should be success.");

	ret = acta.uninstall();
	RUNNER_ASSERT_MSG(ret == 0, "ACTA uninstall should be success.");
}

RUNNER_TEST(T0423_ACTA_UNINSTALL_GLOBAL_APP_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR);
	int ret = acta.uninstall();
	RUNNER_ASSERT_MSG(ret != 0, "ACTA uninstall should be fail.");
}

RUNNER_TEST(T0424_ACTA_UNINSTALL_USER_APP_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID);
	int ret = acta.uninstall();
	RUNNER_ASSERT_MSG(ret != 0, "ACTA uninstall should be fail.");
}

RUNNER_TEST(T0441_ACTA_LAUNCH_GLOBAL_APP_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR);
	int ret = acta.launch(false);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA launch should be success.");
}

RUNNER_TEST(T0442_ACTA_LAUNCH_GLOBAL_APP_WITH_SYS_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR);
	int ret = acta.launch(true);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA launch should be success.");
}

RUNNER_TEST(T0443_ACTA_LAUNCH_USER_APP_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID);
	int ret = acta.launch(false);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA launch should be success.");
}

RUNNER_TEST(T0444_ACTA_INSTALL_USER_APP_WITH_SYS_POSITIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID);
	int ret = acta.launch(true);
	RUNNER_ASSERT_MSG(ret == 0, "ACTA launch should be success.");
}

RUNNER_TEST(T0445_ACTA_LAUNCH_GLOBAL_APP_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR);
	int ret = acta.launch(false);
	RUNNER_ASSERT_MSG(ret != 0, "ACTA launch should be fail.");
}

RUNNER_TEST(T0446_ACTA_LAUNCH_GLOBAL_APP_WITH_SYS_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR);
	int ret = acta.launch(true);
	RUNNER_ASSERT_MSG(ret != 0, "ACTA launch should be fail.");
}

RUNNER_TEST(T0447_ACTA_LAUNCH_USER_APP_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID);
	int ret = acta.launch(false);
	RUNNER_ASSERT_MSG(ret != 0, "ACTA launch should be fail.");
}

RUNNER_TEST(T0448_ACTA_LAUNCH_USER_APP_WITH_SYS_NEGATIVE)
{
	AppCustomTrustAnchor acta(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID);
	int ret = acta.launch(true);
	RUNNER_ASSERT_MSG(ret != 0, "ACTA launch should be fail.");
}
