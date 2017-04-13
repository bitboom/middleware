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

#include <iostream>

#include "test-util.hxx"
#include "test-resource.hxx"

// Launch needs CAP_SYS_ADMIN
TESTCASE(TRUST_ANCHOR_LAUNCH)
{
	auto beforeLs = test::util::ls(TZ_SYS_RO_CA_CERTS);
	auto beforeCat = test::util::cat(TZ_SYS_RO_CA_BUNDLE);

	tanchor::TrustAnchor ta(DUMMY_PKG_ID, APP_CERTS_DIR);
	int ret = ta.install(false);

	std::cout << "##########################################" << std::endl;
	std::cout << "## Before trust-anchor launch#############" << std::endl;
	std::cout << "##########################################" << std::endl;
	ret = test::util::connectSSL("https://google.com");
	TEST_EXPECT(true, ret == 0);

	// pre-condition
	int pid = fork();
	TEST_EXPECT(true, pid >= 0);

	if (pid == 0) {
		TIME_MEASURE_START
		ret = ta.launch(false);
		TIME_MEASURE_END
		TEST_EXPECT(true, ret == 0);

		// check file-system
		auto afterLsChild = test::util::ls(TZ_SYS_RO_CA_CERTS);
		TEST_EXPECT(true, beforeLs != afterLsChild);

		auto afterCatChild = test::util::cat(TZ_SYS_RO_CA_BUNDLE);
		TEST_EXPECT(true, beforeCat != afterCatChild);

		// check SSL communication
		std::cout << "##########################################" << std::endl;
		std::cout << "## After trust-anchor launch(APP)#########" << std::endl;
		std::cout << "##########################################" << std::endl;
		ret = test::util::connectSSL("https://google.com");
		TEST_EXPECT(false, ret == 0);

		exit(0);
	} else {
		auto afterLsParent = test::util::ls(TZ_SYS_RO_CA_CERTS);
		TEST_EXPECT(true, beforeLs == afterLsParent);

		auto afterCatParent = test::util::cat(TZ_SYS_RO_CA_BUNDLE);
		TEST_EXPECT(true, beforeCat == afterCatParent);

		std::cout << "##########################################" << std::endl;
		std::cout << "## After trust-anchor launch(parent)######" << std::endl;
		std::cout << "##########################################" << std::endl;
		ret = test::util::connectSSL("https://google.com");
		TEST_EXPECT(true, ret == 0);
	}
}

TESTCASE(TRUST_ANCHOR_LAUNCH_WITH_SYS)
{
	auto beforeLs = test::util::ls(TZ_SYS_RO_CA_CERTS);
	auto beforeCat = test::util::cat(TZ_SYS_RO_CA_BUNDLE);

	tanchor::TrustAnchor ta(DUMMY_PKG_ID, APP_CERTS_DIR);
	int ret = ta.install(true);

	std::cout << "##########################################" << std::endl;
	std::cout << "## Before trust-anchor launch#############" << std::endl;
	std::cout << "##########################################" << std::endl;
	ret = test::util::connectSSL("https://google.com");
	TEST_EXPECT(true, ret == 0);

	// pre-condition
	int pid = fork();
	TEST_EXPECT(true, pid >= 0);

	if (pid == 0) {
		TIME_MEASURE_START
		ret = ta.launch(true);
		TIME_MEASURE_END
		TEST_EXPECT(true, ret == 0);

		// check file-system
		auto afterLsChild = test::util::ls(TZ_SYS_RO_CA_CERTS);
		TEST_EXPECT(true, beforeLs != afterLsChild);

		auto afterCatChild = test::util::cat(TZ_SYS_RO_CA_BUNDLE);
		TEST_EXPECT(true, beforeCat != afterCatChild);

		// check SSL communication
		std::cout << "###########################################" << std::endl;
		std::cout << "## After trust-anchor launch(APP) with SYS#" << std::endl;
		std::cout << "###########################################" << std::endl;
		ret = test::util::connectSSL("https://google.com");
		TEST_EXPECT(true, ret == 0);

		exit(0);
	} else {
		auto afterLsParent = test::util::ls(TZ_SYS_RO_CA_CERTS);
		TEST_EXPECT(true, beforeLs == afterLsParent);

		auto afterCatParent = test::util::cat(TZ_SYS_RO_CA_BUNDLE);
		TEST_EXPECT(true, beforeCat == afterCatParent);

		std::cout << "##########################################" << std::endl;
		std::cout << "## After trust-anchor launch(parent)######" << std::endl;
		std::cout << "##########################################" << std::endl;
		ret = test::util::connectSSL("https://google.com");
		TEST_EXPECT(true, ret == 0);
	}
}
