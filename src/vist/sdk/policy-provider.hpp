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

#include <vist/sdk/policy-model.hpp>

#include <cstddef>
#include <memory>
#include <unordered_map>

namespace vist {
namespace policy {

class PolicyProvider {
public:
	using FactoryType = PolicyProvider * (*)();

	explicit PolicyProvider(std::string name) noexcept : name(std::move(name)) {}
	virtual ~PolicyProvider() = default;

	inline void add(const std::shared_ptr<PolicyModel>& policy)
	{
		policies[policy->getName()] = policy;
	}

	inline const std::string& getName() const noexcept
	{
		return name;
	}
	static const std::string& getFactoryName() noexcept
	{
		static std::string name = "PolicyFactory";
		return name;
	}

	inline std::size_t size() const noexcept
	{
		return policies.size();
	}

private:
	std::string name;
	std::unordered_map<std::string, std::shared_ptr<PolicyModel>> policies;

	friend class PolicyManager;
};

} // namespace policy
} // namespace vist
