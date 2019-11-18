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

#include <vist/archive.hpp>

#include <string>

namespace vist {
namespace policy {

// TODO: Support various value type
struct PolicyValue final {
	enum class Type {
		Integer,
		String,
		None
	};

	explicit PolicyValue(int value) : type(Type::Integer)
	{
		this->buffer << value;
	}

	explicit PolicyValue(const std::string& value) : type(Type::String)
	{
		this->buffer << value;
	}

	explicit PolicyValue() noexcept = default;
	~PolicyValue() = default;

	PolicyValue(const PolicyValue&) = default;
	PolicyValue& operator=(const PolicyValue&) = default;

	PolicyValue(PolicyValue&&) noexcept = default;
	PolicyValue& operator=(PolicyValue&&) noexcept = default;

	inline Type getType() const noexcept
	{
		return this->type;
	}

	operator int() const
	{
		auto clone = this->buffer;
		int out;
		clone >> out;

		return out;
	}

	operator std::string() const
	{
		auto clone = this->buffer;
		std::string out;
		clone >> out;

		return out;
	}

private:
	Archive buffer;
	Type type = Type::None;
};

} // namespace policy
} // namespace vist
