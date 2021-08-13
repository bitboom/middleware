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

#include <vist/query-builder.hpp>

#include <string>

using namespace vist::tsqb;

namespace vist {
namespace policy {
namespace schema {

struct Admin {
	std::string name;
	int activated = -1;

	DECLARE_COLUMN(Name, "name", &Admin::name);
	DECLARE_COLUMN(Activated, "activated", &Admin::activated);
};

struct PolicyManaged {
	std::string admin;
	std::string policy;
	std::string value;

	DECLARE_COLUMN(Admin, "admin", &PolicyManaged::admin);
	DECLARE_COLUMN(Policy, "policy", &PolicyManaged::policy);
	DECLARE_COLUMN(Value, "value", &PolicyManaged::value);
};

struct PolicyDefinition {
	std::string name;
	std::string ivalue;

	DECLARE_COLUMN(Name, "name", &PolicyDefinition::name);
	DECLARE_COLUMN(Ivalue, "ivalue", &PolicyDefinition::ivalue);
};

DECLARE_TABLE(AdminTable, "ADMIN", Admin::Name, Admin::Activated);
DECLARE_TABLE(PolicyManagedTable, "POLICY_MANAGED", PolicyManaged::Admin,
			  PolicyManaged::Policy,
			  PolicyManaged::Value);
DECLARE_TABLE(PolicyDefinitionTable, "POLICY_DEFINITION", PolicyDefinition::Name,
			  PolicyDefinition::Ivalue);

} // namespace schema
} // namespace policy
} // namespace vist
