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
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>

#include <klay/exception.h>
#include <klay/namespace.h>

namespace runtime {

namespace {

typedef std::pair<std::string, int> NamespacePair;
std::vector<NamespacePair> namespaces = {
    {"mnt",  CLONE_NEWNS},
    {"net",  CLONE_NEWNET},
    {"ipc",  CLONE_NEWIPC},
    {"pid",  CLONE_NEWPID},
    {"uts",  CLONE_NEWUTS},
    {"user", CLONE_NEWUSER},
#ifdef CLONE_NEWCGROUP
    {"cgroup", CLONE_NEWCGROUP},
#endif
};

} // namespace

void Namespace::attach(const pid_t pid)
{
	for (const NamespacePair& ns : namespaces) {
		std::string nspath = "/proc/" + std::to_string(pid) + "/ns/" + ns.first;

		int fd;
		do {
			fd = ::open(nspath.c_str(), O_RDONLY);
		} while (fd == -1 && errno == EINTR);

		if (fd == -1) {
			if (errno != ENOENT) {
				throw runtime::Exception("Failed to open namesapce: " + nspath);
			}
		} else {
			if (::setns(fd, ns.second)) {
				::close(fd);
				throw runtime::Exception("Failed to set namespace: " + nspath);
			}
			::close(fd);
		}
	}
}

void Namespace::unshare(int flags)
{
	if (::unshare(flags)) {
		throw runtime::Exception("Failed to unshare namespace");
	}

	if (flags & CLONE_NEWNS &&
			::mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL) == -1) {
		throw runtime::Exception("Failed to mount root filesystem");
	}
}

} // namespace runtime
