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

#include "observer.h"
#include "policy-context.hxx"

class PolicyClient {
public:
	PolicyClient(PolicyControlContext& context);
	PolicyClient(pid_t pid, uid_t user);

	PolicyClient(const PolicyClient&) = delete;
	PolicyClient(PolicyClient&&) = default;

	~PolicyClient();

	PolicyClient& operator=(PolicyClient&&) = default;
	PolicyClient& operator=(const PolicyClient&) = delete;

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
	int id;
	std::string name;
	uid_t uid;
	std::string key;
};

class ClientManager {
public:
	static void loadAdministrators();

	static void registerAdministrator(const std::string& name, uid_t domain);
	static void deregisterAdministrator(const std::string& name, uid_t domain);

	static void addEventListener(Observer* observer);
	static void removeEventListener(Observer* observer);

private:
	static std::vector<uid_t> getManagedDomainList();

private:
	static Observerable observers;
};

#endif //__DPM_CLIENT_MANAGER_H__
