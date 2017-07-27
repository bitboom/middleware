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

#include <fcntl.h>
#include <unistd.h>

#include <fstream>

#include <klay/error.h>
#include <klay/exception.h>
#include <klay/file-user.h>
#include <klay/filesystem.h>

namespace runtime {

bool FileUser::isUsedAsFD(const std::string &filePath, const pid_t pid, bool isMount)
{
	std::string path = "/proc/" + std::to_string(pid) + "/fd";

	try {
		File file(filePath);

		for (runtime::DirectoryIterator iter(path), end; iter != end;) {
			File cur(File((++iter)->getPath()).readlink());
			try {
				if ((cur.getInode() == file.getInode() || isMount) &&
						cur.getDevice() == file.getDevice()) {
					return true;
				}
			} catch (runtime::Exception &e) {}
		}
	} catch (runtime::Exception &e) {}

	return false;
}

bool FileUser::isUsedAsMap(const std::string &filePath, const pid_t pid, bool isMount)
{
	std::string path = "/proc/" + std::to_string(pid) + "/maps";

	try {
		File mapsFile(path, O_RDONLY), file(filePath);

		dev_t dev, devMin, devMaj;
		dev = file.getDevice();
		devMin = minor(dev);
		devMaj = major(dev);

		std::ifstream mapsStream(path);
		std::string mapsInfo;
		while (std::getline(mapsStream, mapsInfo)) {
			unsigned long long min, maj, inode;

			if (::sscanf(mapsInfo.c_str(), "%*s %*s %*s %llx:%llx %lld",
					&maj, &min, &inode) == 3) {
				if ((dev_t)maj == devMaj && (dev_t)min == devMin &&
						((ino_t)inode == file.getInode() || isMount)) {
					return true;
				}
			}
		}
	} catch (runtime::Exception &e) {}

	return false;
}

bool FileUser::isUsedAsCwd(const std::string &filePath, const pid_t pid, bool isMount)
{
	std::string path = "/proc/" + std::to_string(pid) + "/cwd";

	try {
		File file(filePath), cwd(File(path).readlink());

		if ((cwd.getInode() == file.getInode() || isMount) &&
				cwd.getDevice() == file.getDevice()) {
			return true;
		}
	} catch (runtime::Exception &e) {}

	return false;
}

bool FileUser::isUsedAsRoot(const std::string &filePath, const pid_t pid, bool isMount)
{
	std::string path = "/proc/" + std::to_string(pid) + "/root";

	try {
		File file(filePath), root(File(path).readlink());

		if ((root.getInode() == file.getInode() || isMount) &&
				root.getDevice() == file.getDevice()) {
			return true;
		}
	} catch (runtime::Exception &e) {}

	return false;
}

std::vector<pid_t> FileUser::getList(const std::string &path, bool isMount)
{
	pid_t currentPid = ::getpid();
	std::vector<pid_t> list;

	for (runtime::DirectoryIterator iter("/proc"), end; iter != end;) {
		const std::string name = iter->getName();
		if (!std::isdigit(name[0])) {
			++iter;
			continue;
		}

		pid_t pid = std::stoi(name);
		if (pid == currentPid) {
			++iter;
			continue;
		}

		if (isUsedAsFD(path, pid, isMount) ||
				isUsedAsMap(path, pid, isMount) ||
				isUsedAsCwd(path, pid, isMount) ||
				isUsedAsRoot(path, pid, isMount)) {
			list.push_back(pid);
		}
		++iter;
	}

	return list;
}

} // namespace runtime
