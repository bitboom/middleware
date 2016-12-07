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

#ifndef __DPM_CLIENT_MANAGER_H__
#define __DPM_CLIENT_MANAGER_H__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#include <aul.h>

#include <string>
#include <vector>

#include <klay/exception.h>

class DeviceAdministrator {
public:
	DeviceAdministrator(const DeviceAdministrator&) = delete;
	DeviceAdministrator(DeviceAdministrator&&) = default;
	DeviceAdministrator(const std::string& pkgid, uid_t user, const std::string& pk) :
		name(pkgid), uid(user), key(pk)
	{
	}

	DeviceAdministrator(pid_t pid, uid_t user)
	{
		char pkgid[PATH_MAX];

		if (aul_app_get_pkgid_bypid_for_uid(pid, pkgid, PATH_MAX, user) != 0) {
			int fd = ::open(std::string("/proc/" + std::to_string(pid) + "/cmdline").c_str(), O_RDONLY);
			if (fd == -1) {
				throw runtime::Exception("Unknown PID");
			}

			ssize_t ret, bytes = 0;
			do {
				ret = ::read(fd, &pkgid[bytes], PATH_MAX);
				if (ret != -1) {
					bytes += ret;
				}
			} while ((ret == -1) && (errno == EINTR));

			::close(fd);

			if (ret == -1) {
				throw runtime::Exception("Failed to get admin info");
			}

			pkgid[bytes] = '\0';
		}

		name = pkgid;
		uid = user;
	}

	~DeviceAdministrator()
	{
	}

	DeviceAdministrator& operator=(DeviceAdministrator&&) = default;
	DeviceAdministrator& operator=(const DeviceAdministrator&) = delete;

	inline std::string getName() const
	{
		return name;
	}

	inline uid_t getUid() const
	{
		return uid;
	}

	inline std::string getKey() const
	{
		return key;
	}

private:
	std::string name;
	uid_t uid;
	std::string key;
};

#endif //__DPM_CLIENT_MANAGER_H__
