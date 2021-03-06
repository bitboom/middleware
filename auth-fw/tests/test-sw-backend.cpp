/*
 *  Copyright (c) 2018 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test-sw-backend.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Testcases of PasswordFile(SW-Backend)
 */

#include <klay/testbench.h>

#include <plugin-manager.h>

#include <generic-backend/ipassword-file.h>
#include <sw-backend/password-file.h>

using namespace AuthPasswd;

TESTCASE(T00300_is_support)
{
	PluginManager pm;

	TEST_EXPECT(true, pm.isSupport(BackendType::SW));
	TEST_EXPECT(false, pm.isSupport(BackendType::TZ));
}

TESTCASE(T00301_bind_backend)
{
	PluginManager pm;
	pm.setBackend(BackendType::SW);

	PasswordFileFactory factory = nullptr;
	pm.loadFactory("PasswordFileFactory", factory);

	TEST_EXPECT(true, factory != nullptr);

	std::shared_ptr<IPasswordFile> passwdFile((*factory)(5001));
	TEST_EXPECT(true, passwdFile != nullptr);
}
