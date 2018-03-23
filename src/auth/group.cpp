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

#include <grp.h>
#include <gshadow.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <regex>
#include <memory>

#include <klay/error.h>
#include <klay/exception.h>
#include <klay/auth/group.h>

namespace runtime {

Group::Group(const Group& group) :
	name(group.name), gid(group.gid)
{
}

Group::Group(const std::string& group)
{
	struct group grp, *result;
	int bufsize, ret;

	bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 16384;

	std::unique_ptr<char[]> buf(new char[bufsize]);

	ret = ::getgrnam_r(group.c_str(), &grp, buf.get(), bufsize, &result);
	if (result == NULL) {
		if (ret == 0)
			throw runtime::Exception("Group doesn't exist");
		else
			throw runtime::Exception(runtime::GetSystemErrorMessage(ret));
	}

	name = result->gr_name;
	gid = result->gr_gid;
}

Group::Group(const gid_t group)
{
	struct group grp, *result;
	int bufsize, ret;

	bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 16384;

	std::unique_ptr<char[]> buf(new char[bufsize]);

	ret = ::getgrgid_r(group, &grp, buf.get(), bufsize, &result);
	if (result == NULL) {
		if (ret == 0)
			throw runtime::Exception("Group doesn't exist");
		else
			throw runtime::Exception(runtime::GetSystemErrorMessage(ret));
	}

	name = result->gr_name;
	gid = result->gr_gid;
}

Group::Group() :
	Group(::getgid())
{
}

} // namespace Shadow
