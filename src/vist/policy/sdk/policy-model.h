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

#include <vist/policy/sdk/policy-value.h>

#include <string>
#include <stdexcept>

namespace vist {
namespace policy {

class PolicyModel {
public:
	explicit PolicyModel(std::string name, PolicyValue initial) noexcept :
		name(std::move(name)), initial(std::move(initial)) {}
	virtual ~PolicyModel() = default;

	PolicyModel(const PolicyModel&) = delete;
	PolicyModel& operator=(const PolicyModel&) = delete;

	PolicyModel(PolicyModel&&) = default;
	PolicyModel& operator=(PolicyModel&&) = default;

	inline void set(const PolicyValue& value)
	{
		auto rollback = current;
		current = value;
		ready = true;

		try {
			this->onChanged(value);
		} catch (const std::exception& e) {
			current = rollback;
			ready = false;
			std::rethrow_exception(std::current_exception());
		}
	}

	inline const PolicyValue& get() const
	{
		if (!ready)
			throw std::runtime_error("Policy value should be set once before use.");

		return current;
	}

	virtual void onChanged(const PolicyValue& value) = 0;

	const std::string& getName() const noexcept { return name; }
	const PolicyValue& getInitial() const noexcept { return initial; }

protected:
	std::string name;
	PolicyValue initial;
	PolicyValue current;
	bool ready = false;
};

} // namespace policy
} // namespace vist
