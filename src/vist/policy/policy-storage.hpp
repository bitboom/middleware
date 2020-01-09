/*
 *  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      ttp://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */

#pragma once

#include <vist/database/connection.hpp>
#include <vist/sdk/policy-model.hpp>
#include <vist/sdk/policy-value.hpp>

#include "db-schema.hpp"

#include <memory>
#include <unordered_map>

namespace vist {
namespace policy {

using namespace schema;

class PolicyStorage final {
public:
	explicit PolicyStorage(const std::string& path);

	/// TODO(Sangwan): Consider to support lazy sync
	void sync();

	void syncAdmin();
	void syncPolicyManaged();
	void syncPolicyDefinition();

	inline bool exists(const std::string& policy) const noexcept
	{
		return definitions.find(policy) != definitions.end();
	}

	void enroll(const std::string& admin);
	void disenroll(const std::string& admin);
	void activate(const std::string& admin, bool state = true);
	bool isActivated(const std::string& admin);
	/// Check that none of admins are activated.
	bool isActivated();

	void define(const std::string& policy, const PolicyValue& ivalue);
	void update(const std::string& admin,
				const std::string& policy,
				const PolicyValue& value);

	PolicyValue strictest(const std::shared_ptr<PolicyModel>& policy);

	std::unordered_map<std::string, int> getAdmins() const noexcept;

private:
	std::string getScript(const std::string& name);

	std::shared_ptr<database::Connection> database;

	/// DB Cache objects
	/// TODO(Sangwan): add locking mechanism
	std::unordered_map<std::string, Admin> admins;
	std::unordered_multimap<std::string, PolicyManaged> managedPolicies;
	std::unordered_map<std::string, PolicyDefinition> definitions;
};

} // namespace policy
} // namespace vist
