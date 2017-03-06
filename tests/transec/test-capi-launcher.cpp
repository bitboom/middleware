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
 * @file        test-capi-launcher.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Unit test program of ACTA for launcher
 */

#include <dpl/test/test_runner.h>

#include <acta/app-custom-trust-anchor.h>

#include <unistd.h>

#include "test-util.h"
#include "test-resource.h"

RUNNER_TEST_GROUP_INIT(T0500_CAPI_ACTA_LAUNCER)

// Launch needs CAP_SYS_ADMIN
RUNNER_TEST(T0501_ACTA_LAUNCH)
{
	auto beforeLs = test::util::ls(TZ_SYS_RO_CA_CERTS);
	auto beforeCat = test::util::cat(TZ_SYS_RO_CA_BUNDLE);

	int ret = acta_global_install(DUMMY_PKG_ID, APP_CERTS_DIR, true);

	// pre-condition
	int pid = fork();

	if (pid == 0) {
		ret = acta_global_launch(DUMMY_PKG_ID, APP_CERTS_DIR, true);
		RUNNER_ASSERT_MSG(ret == 0, "ACTA launch should be success.");

		auto afterLsChild = test::util::ls(TZ_SYS_RO_CA_CERTS);
		RUNNER_ASSERT_MSG(beforeLs != afterLsChild, "Failed to launch.");

		auto afterCatChild = test::util::cat(TZ_SYS_RO_CA_BUNDLE);
		RUNNER_ASSERT_MSG(beforeCat != afterCatChild, "Failed to launch.");

	} else {
		auto afterLsParent = test::util::ls(TZ_SYS_RO_CA_CERTS);
		RUNNER_ASSERT_MSG(beforeLs == afterLsParent, "Failed to launch.");

		auto afterCatParent = test::util::cat(TZ_SYS_RO_CA_BUNDLE);
		RUNNER_ASSERT_MSG(beforeCat == afterCatParent, "Failed to launch.");
	}
}
