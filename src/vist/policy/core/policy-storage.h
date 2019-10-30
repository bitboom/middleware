/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/policy/sdk/policy-value.h>

#include "db-schema.h"

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <klay/db/connection.h>

namespace vist {
namespace policy {

using namespace schema;

class PolicyStorage final {
public:
	explicit PolicyStorage(const std::string& path);

	/// TODO(Sangwan): Consider to support lazy sync
	void sync();

	void syncPolicyDefinition();
	void syncAdmin();
	void syncManagedPolicy();

	inline bool exists(const std::string& policy) const noexcept {
		return definitions.find(policy) != definitions.end();
	}

	inline bool isActivated() const noexcept {
		return admins.size() > 0 && managedPolicies.size() > 0;
	}

	void enroll(const std::string& admin, uid_t uid);
	void disenroll(const std::string& admin, uid_t uid);

	void define(int scope, const std::string& policy, int ivalue);
	void update(const std::string& admin, uid_t uid,
				const std::string& policy, const PolicyValue& value);

	PolicyValue strictest(const std::string& policy, uid_t uid = 0);
	/// Return all strictest policy values
	std::unordered_map<std::string, PolicyValue> strictest(uid_t uid = 0);

	/// Admin name can be duplicated
	std::multimap<std::string, int> getAdmins();

private:
	std::string getScript(const std::string& name);
	std::string getAlias(const std::string& name, uid_t uid) const noexcept;
	int getUid(int adminId) const noexcept;

	std::shared_ptr<klay::database::Connection> database;

	/// DB Cache objects
	/// TODO(Sangwan): add locking mechanism
	std::unordered_map<std::string, PolicyDefinition> definitions;
	std::unordered_map<std::string, Admin> admins;
	std::unordered_multimap<int, ManagedPolicy> managedPolicies;
};

} // namespace policy
} // namespace vist
