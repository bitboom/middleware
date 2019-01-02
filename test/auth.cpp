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
		klay::Group group("users");
		klay::Group another(group);
	} catch (klay::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(GetUserTest)
{
	try {
		klay::User user("root");
		klay::User another(user);
	} catch (klay::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(GetGroupNegativeTest)
{
	try {
		klay::Group group("invalid");
	} catch (klay::Exception& e) {
	}
}

TESTCASE(GetGroupNegativeTest2)
{
	try {
		klay::Group group(-1);
	} catch (klay::Exception& e) {
	}
}

TESTCASE(GetUserNegativetest)
{
	try {
		klay::User user("invalid");
	} catch (klay::Exception& e) {
	}
}

TESTCASE(GetUserNegativetest2)
{
	try {
		klay::User user(-1);
	} catch (klay::Exception& e) {
	}
}

TESTCASE(GetCurrentGroupTest)
{
	try {
		klay::Group group;
		klay::Group another(group.getGid());
	} catch (klay::Exception& e) {
	}
}

TESTCASE(GetCurrentUserTest)
{
	try {
		klay::User user;
		klay::User another(user.getUid());
	} catch (klay::Exception& e) {
	}
}
