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
#include <vist/logger.hpp>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

namespace vist {

struct Process {
	static pid_t GetPid()
	{
		return ::getpid();
	}

	/// TODO(Sangwan): Unify the method which get process identifier
	static std::string GetPath(pid_t pid)
	{
		std::string exe = "/proc/" + std::to_string(pid) + "/exe";

		/// c++17 std::filesystem::read_symlink
		std::vector<char> buf(1024);
		errno = 0;
		auto size = ::readlink(exe.c_str(), buf.data(), buf.size());
		if (size == -1) {
			WARN(VIST) << "Failed to get process path by exe: " << exe
					   << ", errno: " << errno;

			std::string cmdline = "/proc/" + std::to_string(pid) + "/cmdline";
			int fd = ::open(cmdline.c_str(), O_RDONLY);
			if (fd == -1)
				THROW(ErrCode::RuntimeError) << "Failed to get process path: " << cmdline;

			errno = 0;
			size = ::read(fd, buf.data(), buf.size());
			::close(fd);

			if (size == -1)
				THROW(ErrCode::RuntimeError) << "Failed to get process path: " << cmdline
											 << ", errno: " << errno;

			buf[size - 1] = '\0';
		}

		return canonicalize(std::string(buf.begin(), buf.begin() + size));
	}

private:
	static std::string canonicalize(std::string&& s)
	{
		auto predicate = [](unsigned char c){ return std::isspace(c) || c == '\0'; };
		auto base = std::find_if(s.begin(), s.end(), predicate);
		s.erase(base, s.end());
		return s;
	}
};

} // namespace vist
