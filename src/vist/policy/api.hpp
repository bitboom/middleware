/*
 *  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/sdk/policy-value.hpp>

#include <string>
#include <unordered_map>

namespace vist {
namespace policy {

struct API {
	static PolicyValue Get(const std::string& policy);
	static std::unordered_map<std::string, PolicyValue> GetAll();

	struct Admin {
		static void Set(const std::string& policy, const PolicyValue& value);

		static void Enroll(const std::string& admin);
		static void Disenroll(const std::string& admin);

		static void Activate(const std::string& admin, bool state = true);
		static bool IsActivated();

		static std::unordered_map<std::string, int> GetAll();
	};
};

} // namespace policy
} // namespace vist
