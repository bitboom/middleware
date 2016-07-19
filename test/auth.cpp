/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <klay/exception.h>
#include <klay/auth/user.h>
#include <klay/auth/group.h>

#include <klay/testbench.h>

TESTCASE(GetGroupTest)
{
	try {
		runtime::Group group("users");
		runtime::Group another(group);
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(GetUserTest)
{
	try {
		runtime::User user("root");
		runtime::User another(user);
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(GetGroupNegativeTest)
{
	try {
		runtime::Group group("invalid");
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(GetGroupNegativeTest2)
{
	try {
		runtime::Group group(-1);
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(GetUserNegativetest)
{
	try {
		runtime::User user("invalid");
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(GetUserNegativetest2)
{
	try {
		runtime::User user(-1);
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(GetCurrentGroupTest)
{
	try {
		runtime::Group group;
		runtime::Group another(group.getGid());
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(GetCurrentUserTest)
{
	try {
		runtime::User user;
		runtime::User another(user.getUid());
	} catch (runtime::Exception& e) {
	}
}
