/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <policyd/sdk/policy-provider.h>
#include <policyd/sdk/policy-value.h>

#include "policy-storage.h"

#include <string>
#include <exception>
#include <memory>
#include <vector>

namespace policyd {

class PolicyManager final {
public:
	PolicyManager(const PolicyManager&) = delete;
	PolicyManager& operator=(const PolicyManager&) = delete;

	PolicyManager(PolicyManager&&) = delete;
	PolicyManager& operator=(PolicyManager&&) = delete;

	static PolicyManager& instance() {
		static PolicyManager manager;
		return manager;
	}

	std::pair<int, int> loadProviders(const std::string& path);
	int loadPolicies();

	void enroll(const std::string& admin, uid_t uid);
	void disenroll(const std::string& admin, uid_t uid);

	void set(const std::string& policy, const PolicyValue& value,
			 const std::string& admin, uid_t uid);
	PolicyValue get(const std::string& policy, uid_t uid);


private:
	explicit PolicyManager() : storage(DB_PATH) {}
	~PolicyManager() = default;

	PolicyStorage storage;
	std::vector<std::shared_ptr<PolicyProvider>> providers;

	std::unordered_map<std::string, std::shared_ptr<GlobalPolicy>> global;
	std::unordered_map<std::string, std::shared_ptr<DomainPolicy>> domain;
};

} // namespace policyd
