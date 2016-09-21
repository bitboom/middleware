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

#ifndef __DPM_POLICY_MANAGER_H__
#define __DPM_POLICY_MANAGER_H__

#include <unistd.h>
#include <sys/types.h>

#include <string>
#include <vector>
#include <unordered_map>

#include "policy.h"
#include "policy-storage.h"

class PolicyManager {
public:
	typedef std::function<bool(int, int)> PolicyComparator;

	PolicyManager(const std::string& base, const std::string& path);
	~PolicyManager();

	PolicyManager(const PolicyManager&) = delete;
	PolicyManager& operator=(const PolicyManager&) = delete;

	void prepareGlobalPolicy();
	void removeGlobalPolicy();
	void prepareUserPolicy(uid_t user);
	void removeUserPolicy(uid_t user);

	void populateStorage(const std::string& name, uid_t uid, bool create);
	void removeStorage(const std::string& name, uid_t uid);
	bool setGlobalPolicy(const std::string& pkgid, uid_t uid, const std::string& name, int value, PolicyComparator& pred);
	bool setUserPolicy(const std::string& pkgid, uid_t uid, const std::string& name, int value, PolicyComparator& pred);
	bool setPolicy(const std::string& client, uid_t uid, const std::string& name, int value);
	int getGlobalPolicy(const std::string& name);
	int getUserPolicy(const std::string& name, uid_t uid);

private:
	std::string buildLocation(uid_t uid, const std::string& name)
	{
		return location + "/" + std::to_string(uid) + "/" + name;
	}

	std::string buildLocation(const std::string& name)
	{
		return location + "/" + name;
	}

	bool updateClientStorage(const std::string& pkgid, uid_t uid, const std::string& name, int value);
	int getStrictGlobalPolicy(const std::string& name, int pivot, PolicyComparator& pred);
	int getStrictUserPolicy(const std::string& name, uid_t uid, int pivot, PolicyComparator& pred);
	void createEffectivePolicy(const std::string& path, int value);
	void setEffectivePolicy(const std::string& path, int value);
	int getEffectivePolicy(const std::string& path);
	void cleanup();

private:
	std::string store;
	std::string location;
	std::vector<std::unique_ptr<PolicyStorage>> storageList;
};

#endif //__DPM_POLICY_MANAGER_H__
