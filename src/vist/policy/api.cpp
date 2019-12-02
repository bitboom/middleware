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

#include <vist/policy/api.hpp>

#include "policy-manager.hpp"

namespace vist {
namespace policy {

PolicyValue API::Get(const std::string& policy)
{
	return PolicyManager::Instance().get(policy);
}

std::unordered_map<std::string, PolicyValue> API::GetAll()
{
	return PolicyManager::Instance().getAll();
}

void API::Admin::Set(const std::string& policy, const PolicyValue& value)
{
	// TODO(Sangwan): Get admin name from peer PID
	PolicyManager::Instance().set(policy, value, "admin");
}

void API::Admin::Enroll(const std::string& admin)
{
	PolicyManager::Instance().enroll(admin);
}

void API::Admin::Disenroll(const std::string& admin)
{
	PolicyManager::Instance().disenroll(admin);
}

void API::Admin::Activate(const std::string& admin, bool state)
{
	PolicyManager::Instance().activate(admin, state);
}

std::vector<std::string> API::Admin::GetAll()
{
	return PolicyManager::Instance().getAdmins();
}

} // namespace policy
} // namespace vist
