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

#ifndef __RUNTIME_GROUP_H__
#define __RUNTIME_GROUP_H__

#include <limits.h>
#include <sys/types.h>

#include <string>

#include <klay/klay.h>

#ifndef INVALID_GID
#define INVALID_GID             UINT_MAX
#endif

namespace runtime {

class KLAY_EXPORT Group final {
public:
	Group(const std::string& name);
	Group(const gid_t group);
	Group(const Group& group);
	Group();

	const std::string& getName() const
	{
		return name;
	}

	const gid_t getGid() const
	{
		return gid;
	}

private:
	std::string name;
	gid_t gid;
};

} // namespace runtime

#endif //__RUNTIME_GROUP_H__
