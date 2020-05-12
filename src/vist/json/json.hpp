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
 *     - Leaf structure: Int, String (To be added: Bool, Null)
 *     - Composite structure: Object (To be added: Array)
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

#include <vist/json/value.hpp>
#include <vist/json/object.hpp>

#include <sstream>
#include <stdexcept>
#include <string>

namespace vist {
namespace json {

struct Json {
	Value& operator[](const std::string& key)
	{
		if (!this->root.exist(key))
			this->root.pairs[key] = std::make_shared<Value>();

		return *(this->root.pairs[key]);
	}

	// Return the number of 1-depth's elements.
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

	void push(const std::string& key, Json& child)
	{
		auto object = std::make_shared<Object>();
		object->pairs = std::move(child.root.pairs);
		this->root.pairs[key] = object; 
	}

	Json pop(const std::string& key)
	{
		if (!this->root.exist(key))
			throw std::runtime_error("Not exist key.");

		if (auto downcast = std::dynamic_pointer_cast<Object>(this->root.pairs[key]);
			downcast == nullptr)
			throw std::runtime_error("Mismatched type.");
		else {
			Json json;
			json.root.pairs = std::move(downcast->pairs);
			this->root.pairs.erase(key);
			return json;
		}
	}

	Object root;
};

} // namespace json
} // namespace vist
