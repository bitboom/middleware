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

#pragma once

#include <vist/json/value.hpp>

#include <string>
#include <unordered_map>

namespace vist {
namespace json {

struct Object : public Value {
	std::size_t size() const noexcept
	{
		return this->pairs.size();
	}

	bool exist(const std::string& key) const noexcept
	{
		return this->pairs.find(key) != this->pairs.end();
	}

	std::string serialize() const override
	{
		std::stringstream ss;
		ss << "{ ";

		std::size_t i = 0;
		for (const auto& [key, value] : pairs) {
			ss << "\"" << key << "\": ";
			ss << value->serialize();

			if (i++ < pairs.size() - 1)
				ss << ",";

			ss << " ";
		}
		ss << "}";

		return ss.str();
	}

	Value& operator[](const char* key)
	{
		if (!this->exist(key))
			this->pairs[key] = std::make_shared<Value>();

		return *(this->pairs[key]);
	}

	std::unordered_map<std::string, std::shared_ptr<Value>> pairs;
};

} // namespace json
} // namespace vist
