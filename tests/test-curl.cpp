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
 * @file        test-curl.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Unit test program of Curl
 */

#include <tanchor/trust-anchor.hxx>

#include <klay/testbench.h>

#include <unistd.h>

#include <iostream>

#include "test-util.hxx"
#include "test-resource.hxx"

TESTCASE(CONNECT_SSL)
{
	int ret = test::util::connectSSL("https://google.com");
	TEST_EXPECT(true, ret == 0);

	if (ret != 0)
		std::cout << "Check wifi connection.." << std::endl;
}

TESTCASE(TRUST_ANCHOR_LAUNCH)
{
	tanchor::TrustAnchor ta(DUMMY_PKG_ID, APP_CERTS_DIR);
	int ret = ta.install(false);
	TEST_EXPECT(true, ret == 0);

	std::cout << "##########################################" << std::endl;
	std::cout << "## Before trust-anchor launch#############" << std::endl;
	std::cout << "##########################################" << std::endl;
	ret = test::util::connectSSL("https://google.com");
	TEST_EXPECT(true, ret == 0);

	// pre-condition
	int pid = fork();
	TEST_EXPECT(true, pid >= 0);

	if (pid == 0) {
		ret = ta.launch();
		TEST_EXPECT(true, ret == 0);

		// check SSL communication
		std::cout << "##########################################" << std::endl;
		std::cout << "## After trust-anchor launch(APP)#########" << std::endl;
		std::cout << "##########################################" << std::endl;
		ret = test::util::connectSSL("https://google.com");
		TEST_EXPECT(false, ret == 0);

		exit(0);
	} else {
		std::cout << "##########################################" << std::endl;
		std::cout << "## After trust-anchor launch(parent)######" << std::endl;
		std::cout << "##########################################" << std::endl;
		ret = test::util::connectSSL("https://google.com");
		TEST_EXPECT(true, ret == 0);
	}
}

TESTCASE(TRUST_ANCHOR_LAUNCH_WITH_SYS)
{
	tanchor::TrustAnchor ta(DUMMY_PKG_ID, APP_CERTS_DIR);
	int ret = ta.install(true);
	TEST_EXPECT(true, ret == 0);

	std::cout << "##########################################" << std::endl;
	std::cout << "## Before trust-anchor launch#############" << std::endl;
	std::cout << "##########################################" << std::endl;
	ret = test::util::connectSSL("https://google.com");
	TEST_EXPECT(true, ret == 0);

	// pre-condition
	int pid = fork();
	TEST_EXPECT(true, pid >= 0);

	if (pid == 0) {
		ret = ta.launch();
		TEST_EXPECT(true, ret == 0);

		// check SSL communication
		std::cout << "###########################################" << std::endl;
		std::cout << "## After trust-anchor launch(APP) with SYS#" << std::endl;
		std::cout << "###########################################" << std::endl;
		ret = test::util::connectSSL("https://google.com");
		TEST_EXPECT(true, ret == 0);

		exit(0);
	} else {
		std::cout << "##########################################" << std::endl;
		std::cout << "## After trust-anchor launch(parent)######" << std::endl;
		std::cout << "##########################################" << std::endl;
		ret = test::util::connectSSL("https://google.com");
		TEST_EXPECT(true, ret == 0);
	}
}
