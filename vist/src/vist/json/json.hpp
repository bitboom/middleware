/*
 *  Copyright (c) 2020 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the language governing permissions and
 *  limitations under the License
 */
/*
 * Usable JSON header-only library.
 *   - Applied design pattern: Composite pattern
 *     - Component structure: Value
 *     - Leaf structure: Int, String, Bool, Null
 *     - Composite structure: Array, Object
 */
/*
 * Usage:
 *     // Set json object
 *     Json json;
 *     json["name"] = "sangwan";
 *     json["age"] = 99;
 *
 *     // Get json value
 *     std::string name = json["name"];
 *     int age = json["age"];
 *
 *     // Serialize json value
 *     std::string serialized = json.serialize();
 */

#pragma once

#include <vist/json/array.hpp>
#include <vist/json/object.hpp>
#include <vist/json/value.hpp>
#include <vist/string.hpp>

#include <sstream>
#include <stdexcept>
#include <string>

namespace vist {
namespace json {

struct Json {
	static Json Parse(const std::string& raw)
	{
		Json json;
		json.deserialize(raw);
		return json;
	}

	Value& operator[](const std::string& key)
	{
		if (!this->root.exist(key))
			this->root.pairs[key] = std::make_shared<Value>();

		return *(this->root.pairs[key]);
	}

	template <typename CompositeType>
	void push(const std::string& key, CompositeType& child)
	{
		auto value = std::make_shared<Value>();
		auto composite = std::make_shared<CompositeType>();
		if constexpr(std::is_same_v<CompositeType, Array>)
			composite->buffer = std::move(child.buffer);
		else if constexpr(std::is_same_v<CompositeType, Object>)
			composite->pairs = std::move(child.pairs);
		else
			static_assert(dependent_false<CompositeType>::value, "Only Composite type supported.");

		value->leaf = composite;
		this->root.pairs[key] = std::move(value);
	}

	template <typename CompositeType>
	CompositeType& get(const std::string& key)
	{
		return this->root.get<CompositeType>(key);
	}

	bool exist(const std::string& key)
	{
		return this->root.exist(key);
	}

	std::size_t size() const noexcept
	{
		return this->root.size();
	}

	bool exist(const std::string& key) const noexcept
	{
		return this->root.exist(key);
	}

	std::string serialize() const
	{
		return this->root.serialize();
	}

	void deserialize(const std::string& dumped)
	{
		this->root.deserialize(dumped);
	}

	Object root;
};

} // namespace json
} // namespace vist
