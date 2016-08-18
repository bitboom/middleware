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
#include <unistd.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include <klay/db/column.h>
#include <klay/db/statement.h>
#include <klay/db/connection.h>

class DeviceAdministrator {
public:
	DeviceAdministrator(const DeviceAdministrator&) = delete;
	DeviceAdministrator(DeviceAdministrator&&) = default;
	DeviceAdministrator(const std::string& name, uid_t uid, const std::string& key);

	~DeviceAdministrator();

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

class DeviceAdministratorManager {
public:
	typedef std::vector<DeviceAdministrator> DeviceAdministratorList;

	DeviceAdministratorManager(const std::string& path);
	DeviceAdministratorManager(const DeviceAdministratorManager&) = delete;
	DeviceAdministratorManager& operator=(const DeviceAdministratorManager&) = delete;

	DeviceAdministrator enroll(const std::string& name, uid_t uid);
	void disenroll(const std::string& name, uid_t uid);

	DeviceAdministratorList::iterator begin()
	{
		return deviceAdministratorList.begin();
	}

	DeviceAdministratorList::iterator end()
	{
		return deviceAdministratorList.end();
	}

private:
	void prepareRepository();
	std::string generateKey();

private:
	std::string repository;
	DeviceAdministratorList deviceAdministratorList;
};

#endif //__DPM_CLIENT_MANAGER_H__
