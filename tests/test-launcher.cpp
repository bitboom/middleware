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
 * @file        test-launcher.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Unit test program of Trust Anchor for launcher
 */

#include <tanchor/trust-anchor.hxx>

#include <klay/testbench.h>

#include <unistd.h>

#include "test-util.hxx"
#include "test-resource.hxx"

// Launch needs CAP_SYS_ADMIN
TESTCASE(TRUST_ANCHOR_LAUNCH)
{
	auto beforeLs = test::util::ls(TZ_SYS_RO_CA_CERTS);
	auto beforeCat = test::util::cat(TZ_SYS_RO_CA_BUNDLE);

	tanchor::TrustAnchor ta(DUMMY_PKG_ID, APP_CERTS_DIR);
	int ret = ta.install(true);

	// pre-condition
	int pid = fork();

	if (pid == 0) {
		ret = ta.launch(true);
		TEST_EXPECT(true, ret == 0);

		auto afterLsChild = test::util::ls(TZ_SYS_RO_CA_CERTS);
		TEST_EXPECT(true, beforeLs != afterLsChild);

		auto afterCatChild = test::util::cat(TZ_SYS_RO_CA_BUNDLE);
		TEST_EXPECT(true, beforeCat != afterCatChild);
	} else {
		auto afterLsParent = test::util::ls(TZ_SYS_RO_CA_CERTS);
		TEST_EXPECT(true, beforeLs == afterLsParent);

		auto afterCatParent = test::util::cat(TZ_SYS_RO_CA_BUNDLE);
		TEST_EXPECT(true, beforeCat == afterCatParent);
	}
}
