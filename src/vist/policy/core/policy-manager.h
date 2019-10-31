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

#include <vist/policy/sdk/policy-provider.h>
#include <vist/policy/sdk/policy-value.h>

#include "policy-storage.h"

#include <exception>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <gtest/gtest_prod.h>

namespace vist {
namespace policy {

class PolicyManager final {
public:
	PolicyManager(const PolicyManager&) = delete;
	PolicyManager& operator=(const PolicyManager&) = delete;

	PolicyManager(PolicyManager&&) = delete;
	PolicyManager& operator=(PolicyManager&&) = delete;

	static PolicyManager& Instance()
	{
		static PolicyManager manager;
		return manager;
	}

	void enroll(const std::string& admin);
	void disenroll(const std::string& admin);

	void set(const std::string& policy,
			 const PolicyValue& value,
			 const std::string& admin);
	PolicyValue get(const std::string& policy);
	std::unordered_map<std::string, PolicyValue> getAll();

	std::vector<std::string> getAdmins();

private:
	explicit PolicyManager();
	~PolicyManager() = default;

	std::pair<int, int> loadProviders(const std::string& path);
	int loadPolicies();

	PolicyStorage storage;
	std::vector<std::unique_ptr<PolicyProvider>> providers;

	/// Policy-Provider
	std::unordered_map<std::string, std::string> policies;

	FRIEND_TEST(PolicyCoreTests, policy_loader);
};

} // namespace policy
} // namespace vist
