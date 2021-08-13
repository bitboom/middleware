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

#include <vist/stringfy.hpp>

#include <string>

namespace vist {
namespace policy {

struct PolicyValue final {
	explicit PolicyValue() noexcept = default;
	explicit PolicyValue(int value) : stringfied(Stringify::Dump(value)) {}
	explicit PolicyValue(const std::string& value, bool dumped = false)
		: stringfied(dumped ? value : Stringify::Dump(value)) {}
	~PolicyValue() = default;

	PolicyValue(const PolicyValue&) = default;
	PolicyValue& operator=(const PolicyValue&) = default;

	PolicyValue(PolicyValue&&) noexcept = default;
	PolicyValue& operator=(PolicyValue&&) noexcept = default;

	inline std::string dump() const noexcept
	{
		return this->stringfied;
	}

	inline Stringify::Type getType() const
	{
		return Stringify::GetType(this->stringfied);
	}

	operator int() const
	{
		return Stringify::Restore(this->stringfied);
	}

	operator std::string() const
	{
		return Stringify::Restore(this->stringfied);
	}

private:
	std::string stringfied;
};

} // namespace policy
} // namespace vist
