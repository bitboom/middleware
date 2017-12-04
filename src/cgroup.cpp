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
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>

#include <regex>
#include <fstream>

#include <klay/cgroup.h>
#include <klay/exception.h>
#include <klay/filesystem.h>

#define NAME_PATTERN "^[A-Za-z_][A-Za-z0-9_-]*"
#define PATH_PATTERN "(/*[A-Za-z_][A-Za-z0-9_-]*)*"

namespace runtime {

bool Cgroup::existSubsystem(const std::string& name)
{
	try {
		if (!std::regex_match(name, std::regex(NAME_PATTERN))) {
			return false;
		}
	} catch (std::runtime_error &e) {
		throw runtime::Exception("Unexpected regex error");
	}

	runtime::File dir("/sys/fs/cgroup/" + name);
	if (dir.exists()) {
		if (dir.isDirectory()) {
			return true;
		}
		throw runtime::Exception("Invalid subsystem name");
	}

	return false;
}

void Cgroup::createSubsystem(const std::string& name)
{
	try {
		if (!std::regex_match(name, std::regex(NAME_PATTERN))) {
			throw runtime::Exception("Invalid subsystem name");
		}
	} catch (std::runtime_error &e) {
		throw runtime::Exception("Unexpected regex error");
	}

	if (existSubsystem(name)) {
		return;
	}

	runtime::File subsystem("/sys/fs/cgroup/" + name);
	if (::mount(NULL, "/sys/fs/cgroup/", NULL, MS_REMOUNT |
			MS_NOSUID | MS_NOEXEC | MS_NODEV | MS_STRICTATIME,
			"mode=755")) {
		throw runtime::Exception("Failed to remount cgroupfs as the writable");
	}

	if (!subsystem.exists()) {
		subsystem.makeDirectory(true);
	}

	if (::mount(name.c_str(), subsystem.getPath().c_str(),
			"cgroup", MS_NODEV | MS_NOSUID | MS_NOEXEC,
			("none,name=" + name).c_str())) {
		subsystem.remove(false);
		throw runtime::Exception("Failed to mount cgroup subsystem");
	}

	if (::mount(NULL, "/sys/fs/cgroup/", NULL, MS_REMOUNT | MS_RDONLY |
			MS_NOSUID | MS_NOEXEC | MS_NODEV | MS_STRICTATIME,
			"mode=755")) {
		throw runtime::Exception("Failed to remount cgroupfs as the read-only");
	}
}

void Cgroup::destroySubsystem(const std::string& name)
{
	if (!existSubsystem(name)) {
		 throw runtime::Exception("No such subsystem");
	}

	if (::mount(NULL, "/sys/fs/cgroup/", NULL, MS_REMOUNT |
			MS_NOSUID | MS_NOEXEC | MS_NODEV | MS_STRICTATIME,
			"mode=755")) {
		throw runtime::Exception("Failed to remount cgroupfs as the writable");
	}

	runtime::File subsystem("/sys/fs/cgroup/" + name);
	::umount2(subsystem.getPath().c_str(), MNT_EXPIRE);

	subsystem.remove(false);

	if (::mount(NULL, "/sys/fs/cgroup/", NULL, MS_REMOUNT | MS_RDONLY |
			MS_NOSUID | MS_NOEXEC | MS_NODEV | MS_STRICTATIME,
			"mode=755")) {
		throw runtime::Exception("Failed to remount cgroupfs as the read-only");
	}
}

bool Cgroup::exist(const std::string& subsystem, const std::string& path)
{
	try {
		if (!std::regex_match(path, std::regex(PATH_PATTERN))) {
			return false;
		}
	} catch (std::runtime_error &e) {
		throw runtime::Exception("Unexpected regex error");
	}

	runtime::File dir("/sys/fs/cgroup/" + subsystem + "/" + path);
	if (dir.exists()) {
		if (dir.isDirectory()) {
			return true;
		}
		throw runtime::Exception("Invalid path");
	}

	return false;
}

void Cgroup::create(const std::string& subsystem, const std::string& path)
{
	try {
		if (!std::regex_match(path, std::regex(PATH_PATTERN))) {
			throw runtime::Exception("Invalid path");
		}
	} catch (std::runtime_error &e) {
		throw runtime::Exception("Unexpected regex error");
	}

	if (exist(subsystem, path)) {
		return;
	}

	runtime::File dir("/sys/fs/cgroup/" + subsystem + "/" + path);
	dir.makeDirectory(true);
}

void Cgroup::destroy(const std::string& subsystem, const std::string& path)
{
	if (!exist(subsystem, path)) {
		throw runtime::Exception("No such path in subsystem");
	}

	runtime::File dir("/sys/fs/cgroup/" + subsystem + "/" + path);
	dir.remove(false);
}

void Cgroup::addProcess(const std::string& subsystem, const std::string& path, const pid_t pid)
{
	if (!exist(subsystem, path)) {
		throw runtime::Exception("No such path in subsystem");
	}

	std::ofstream ofs("/sys/fs/cgroup/" + subsystem + "/" + path +
						"/tasks");

	ofs << pid << std::endl;
}

std::vector<pid_t> Cgroup::getProcessList(const std::string& subsystem, const std::string& path)
{
	std::vector<pid_t> ret;
	std::ifstream ifs("/sys/fs/cgroup/" + subsystem + "/" + path +
						"/tasks");

	while (ifs.good()) {
		pid_t pid;
		ifs >> pid;
		ret.push_back(pid);
	}

	return ret;
}

const std::string Cgroup::getPath(const std::string& subsystem, const pid_t pid)
{
	std::ifstream ifs("/proc/" + std::to_string(pid) + "/cgroup");
	std::string ret = "/", line;

	while (std::getline(ifs, line)) {
		std::stringstream lineStream(line);
		std::string name;

		//the first getline is for removing the first argument
		std::getline(lineStream, name, ':');
		std::getline(lineStream, name, ':');

		if (name == subsystem || name == "name=" + subsystem) {
			ret = line.substr(line.find('/'));
		}
	}
	return ret;
}

} // namespace runtime
