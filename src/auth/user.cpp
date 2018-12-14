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

#include <pwd.h>
#include <shadow.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <regex>
#include <memory>

#include <klay/error.h>
#include <klay/exception.h>
#include <klay/auth/user.h>

namespace klay {

User::User(const User& user) :
	name(user.name), uid(user.uid), gid(user.gid)
{
}

User::User(const std::string& user)
{
	struct passwd pwd, *result;
	int bufsize, ret;

	bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 16384;

	std::unique_ptr<char[]> buf(new char[bufsize]);

	ret = ::getpwnam_r(user.c_str(), &pwd, buf.get(), bufsize, &result);
	if (result == NULL) {
		if (ret == 0)
			throw runtime::Exception("User " + user + " doesn't exist");
		else
			throw runtime::Exception(runtime::GetSystemErrorMessage(ret));
	}

	name = result->pw_name;
	uid = result->pw_uid;
	gid = result->pw_gid;
}

User::User(const uid_t user)
{
	struct passwd pwd, *result;
	int bufsize, ret;

	bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 16384;

	std::unique_ptr<char[]> buf(new char[bufsize]);
	ret = ::getpwuid_r(user, &pwd, buf.get(), bufsize, &result);
	if (result == NULL) {
		if (ret == 0)
			throw runtime::Exception("User " + std::to_string(user) + "doesn't exist");
		else
			throw runtime::Exception(runtime::GetSystemErrorMessage(ret));
	}

	name = result->pw_name;
	uid = result->pw_uid;
	gid = result->pw_gid;
}

User::User() :
	User(::getuid())
{
}

} // namespace klay
