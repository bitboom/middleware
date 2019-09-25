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

#include "db-schema.h"

#include <memory>
#include <vector>
#include <unordered_map>

#include <klay/db/connection.h>

namespace policyd {

using namespace schema;

class PolicyStorage final {
public:
	explicit PolicyStorage(const std::string& path);

	void sync();

	inline bool exists(const std::string& policy) noexcept {
		return definitions.find(policy) != definitions.end();
	}

private:
	void syncPolicyDefinition();
	void syncAdmin();
	void syncManagedPolicy();

	std::shared_ptr<klay::database::Connection> database;

	/// DB Cache objects
	std::unordered_map<std::string, PolicyDefinition> definitions;
	std::vector<Admin> admins;
	std::vector<ManagedPolicy> managedPolicies;
};

} // namespace policyd
