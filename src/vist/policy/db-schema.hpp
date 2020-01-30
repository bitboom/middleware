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
};

struct PolicyManaged {
	std::string admin;
	std::string policy;
	std::string value;
};

struct PolicyDefinition {
	std::string name;
	std::string ivalue;
};

inline Table admin { "ADMIN", Column("name", &Admin::name),
							  Column("activated", &Admin::activated) };

inline Table policyManaged { "POLICY_MANAGED", Column("admin", &PolicyManaged::admin),
											   Column("policy", &PolicyManaged::policy),
											   Column("value", &PolicyManaged::value) };

inline Table policyDefinition { "POLICY_DEFINITION",
								Column("name", &PolicyDefinition::name),
								Column("ivalue", &PolicyDefinition::ivalue) };

} // namespace schema
} // namespace policy
} // namespace vist
