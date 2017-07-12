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
/*
 * @file        test-capi-installer.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Unit test program of Trust Anchor CAPI for installer
 */

#include <tanchor/trust-anchor.h>

#include <klay/testbench.h>

#include "test-resource.hxx"

TESTCASE(CAPI_TRUST_ANCHOR_INSTALL_POSITIVE)
{
	int ret = trust_anchor_install(DUMMY_PKG_ID, PKG_CERTS_DIR, DUMMY_UID, false);
	TEST_EXPECT(true, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(CAPI_TRUST_ANCHOR_INSTALL_WITH_SYS_POSITIVE)
{
	int ret = trust_anchor_install(DUMMY_PKG_ID, PKG_CERTS_DIR, DUMMY_UID, true);
	TEST_EXPECT(true, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(CAPI_TRUST_ANCHOR_INSTALL_NEGATIVE)
{
	int ret = trust_anchor_install(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID, false);
	TEST_EXPECT(false, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(CAPI_TRUST_ANCHOR_INSTALL_WITH_SYS_NEGATIVE)
{
	int ret = trust_anchor_install(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID, true);
	TEST_EXPECT(false, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(CAPI_TRUST_ANCHOR_UNINSTALL_POSITIVE)
{
	int ret = trust_anchor_install(DUMMY_PKG_ID, PKG_CERTS_DIR, DUMMY_UID, false);
	TEST_EXPECT(true, ret == TRUST_ANCHOR_ERROR_NONE);

	ret = trust_anchor_uninstall(DUMMY_PKG_ID, PKG_CERTS_DIR, DUMMY_UID);
	TEST_EXPECT(true, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(CAPI_TRUST_ANCHOR_UNINSTALL_NEGATIVE)
{
	int ret = trust_anchor_uninstall(DUMMY_PKG_ID, DUMMY_CERTS_DIR, DUMMY_UID);
	TEST_EXPECT(false, ret == TRUST_ANCHOR_ERROR_NONE);
}
