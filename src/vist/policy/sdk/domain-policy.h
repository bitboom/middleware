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

#include "policy-model.h"

#include <exception>
#include <string>
#include <unordered_map>

#include <sys/types.h>

namespace vist {
namespace policy {

class DomainPolicy : public PolicyModel {
public:
	explicit DomainPolicy(std::string name, PolicyValue initial) noexcept :
		PolicyModel(std::move(name), std::move(initial)) {}
	virtual ~DomainPolicy() = default;

	DomainPolicy(DomainPolicy&&) = default;
	DomainPolicy& operator=(DomainPolicy&&) = default;

	inline void set(uid_t domain, const PolicyValue& value) {
		current[domain] = value;

		try {
			this->onChanged(domain, value);
		} catch (const std::exception& e) {
			current.erase(domain);
			std::rethrow_exception(std::current_exception());
		}
	}

	inline const PolicyValue& get(uid_t domain) const {
		if (!current.count(domain))
			throw std::runtime_error("Policy value should be set once before use.");

		return current.at(domain);
	}

	virtual void onChanged(uid_t domain, const PolicyValue& value) = 0;

private:
	std::unordered_map<uid_t, PolicyValue> current;
};

} // namespace policy
} // namespace vist
