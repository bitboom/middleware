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
 * @file        test-installer.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Unit test program of Trust Anchor for installer
 */

#include <tanchor/trust-anchor.hxx>

#include <klay/testbench.h>

#include <sched.h>
#include <unistd.h>

#include <iostream>

#include "test-resource.hxx"
#include "test-util.hxx"

TESTCASE(TRUST_ANCHOR_INSTALL_POSITIVE)
{
	tanchor::TrustAnchor ta(DUMMY_PKG_ID, DUMMY_UID);
	int ret = ta.install(PKG_CERTS_DIR, false);
	TEST_EXPECT(true, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(TRUST_ANCHOR_INSTALL_WITH_SYS_POSITIVE)
{
	tanchor::TrustAnchor ta(DUMMY_PKG_ID, DUMMY_UID);
	int ret = ta.install(PKG_CERTS_DIR, true);
	TEST_EXPECT(true, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(TRUST_ANCHOR_INSTALL_NEGATIVE)
{
	tanchor::TrustAnchor ta(DUMMY_PKG_ID, DUMMY_UID);
	int ret = ta.install(DUMMY_CERTS_DIR, false);
	TEST_EXPECT(false, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(TRUST_ANCHOR_INSTALL_WITH_SYS_NEGATIVE)
{
	tanchor::TrustAnchor ta(DUMMY_PKG_ID, DUMMY_UID);
	int ret = ta.install(DUMMY_CERTS_DIR, true);
	TEST_EXPECT(false, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(TRUST_ANCHOR_UNINSTALL_POSITIVE)
{
	tanchor::TrustAnchor ta(DUMMY_PKG_ID, DUMMY_UID);
	int ret = ta.install(PKG_CERTS_DIR, false);
	TEST_EXPECT(true, ret == TRUST_ANCHOR_ERROR_NONE);

	ret = ta.uninstall();
	TEST_EXPECT(true, ret == TRUST_ANCHOR_ERROR_NONE);
}

TESTCASE(TRUST_ANCHOR_UNINSTALL_NEGATIVE)
{
	tanchor::TrustAnchor ta(DUMMY_PKG_ID, DUMMY_UID);
	int ret = ta.uninstall();
	TEST_EXPECT(false, ret == TRUST_ANCHOR_ERROR_NONE);
}
