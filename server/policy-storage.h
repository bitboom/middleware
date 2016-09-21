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

#ifndef __DPM_POLICY_STORAGE_H__
#define __DPM_POLICY_STORAGE_H__

#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "policy.h"

#include <klay/xml/document.h>

class PolicyStorage {
public:
	typedef std::unordered_map<std::string, Policy>::iterator PolicyIterator;

	PolicyStorage(const std::string& storage, const std::string& pkgid, uid_t uid, bool create = true);
	~PolicyStorage();

	PolicyStorage(const PolicyStorage&) = delete;
	PolicyStorage& operator=(const PolicyStorage&) = delete;

	void setPolicy(const std::string& name, int value);
	int getPolicy(const std::string& name);

	bool isAssociated(uid_t uid) const
	{
		return (uid == user);
	}

	bool isAssociated(const std::string& pkgid, uid_t uid) const
	{
		return ((pkgid == owner) && (uid == user));
	}

	PolicyIterator begin()
	{
		return policyMap.begin();
	}

	PolicyIterator end()
	{
		return policyMap.end();
	}

	void flush();

	void remove();

private:
	uid_t user;
	std::string owner;
	std::string location;
	std::unique_ptr<xml::Document> data;
	std::unordered_map<std::string, Policy> policyMap;
};

#endif //__DPM_POLICY_STORAGE_H__
