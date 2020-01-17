/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#pragma once

#include <vist/exception.hpp>

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

namespace vist {

struct Process {
	static pid_t GetPid()
	{
		return ::getpid();
	}

	static std::string GetPath(pid_t pid)
	{
		std::string cmdline = "/proc/" + std::to_string(pid) + "/exe";

		/// c++17 std::filesystem::read_symlink
		std::vector<char> buf(1024);
		auto size = ::readlink(cmdline.c_str(), buf.data(), buf.size());
		if (size == -1)
			THROW(ErrCode::RuntimeError) << "Failed to get process path: " << errno;

		return std::string(buf.begin(), buf.begin() + size);
	}
};

} // namespace vist
