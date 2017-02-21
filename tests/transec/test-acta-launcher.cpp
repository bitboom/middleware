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
 * @file        test-actc-launcher.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Unit test program of ACTA for launcher
 */

#include <dpl/test/test_runner.h>

#include <AppCustomTrustAnchor.h>

#include <cstdio>
#include <sched.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <vector>

#include "test-resource.h"

RUNNER_TEST_GROUP_INIT(T0600_API_ACTA_LAUNCER)

using namespace transec;

namespace {

std::string ls(const char *path)
{
	using FilePtr = std::unique_ptr<FILE, decltype(&::pclose)>;
	std::string cmd("/bin/ls ");
	cmd.append(path);

	FilePtr ls(::popen(cmd.c_str(), "r"), ::pclose);
	if (ls == nullptr)
		return std::string();

	std::vector<char> buf(1024);
	std::string ret;
	while (::fgets(buf.data(), buf.size(), ls.get()))
		ret.append(buf.data());

	return ret;
}

std::string cat(const char *path)
{
	using FilePtr = std::unique_ptr<FILE, decltype(&::pclose)>;
	std::string cmd("/bin/cat ");
	cmd.append(path);

	FilePtr ls(::popen(cmd.c_str(), "r"), ::pclose);
	if (ls == nullptr)
		return std::string();

	std::vector<char> buf(1024);
	std::string ret;
	while (::fgets(buf.data(), buf.size(), ls.get()))
		ret.append(buf.data());

	return ret;
}

} // namesapce anonymous

// Launch needs CAP_SYS_ADMIN
RUNNER_TEST(T0601_ACTA_LAUNCH)
{
	auto beforeLs = ::ls(TZ_SYS_RO_CA_CERTS);
	auto beforeCat = ::cat(TZ_SYS_RO_CA_BUNDLE);

	AppCustomTrustAnchor acta(DUMMY_PKG_ID, APP_CERTS_DIR);
	int ret = acta.install(false);

	// pre-condition
	int pid = fork();

	if (pid == 0) {
		ret = acta.launch(false);
		RUNNER_ASSERT_MSG(ret == 0, "ACTA launch should be success.");

		auto afterLsChild = ::ls(TZ_SYS_RO_CA_CERTS);
		RUNNER_ASSERT_MSG(beforeLs != afterLsChild, "Failed to launch.");

		auto afterCatChild = ::cat(TZ_SYS_RO_CA_BUNDLE);
		RUNNER_ASSERT_MSG(beforeCat != afterCatChild, "Failed to launch.");

	} else {
		auto afterLsParent = ::ls(TZ_SYS_RO_CA_CERTS);
		RUNNER_ASSERT_MSG(beforeLs == afterLsParent, "Failed to launch.");

		auto afterCatParent = ::cat(TZ_SYS_RO_CA_BUNDLE);
		RUNNER_ASSERT_MSG(beforeCat == afterCatParent, "Failed to launch.");
	}
}
