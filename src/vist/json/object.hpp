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

#include <vist/json/util.hpp>
#include <vist/json/value.hpp>
#include <vist/string.hpp>

#include <string>
#include <unordered_map>

namespace vist {
namespace json {

struct Object : public Value {
	static Object Create(Value& value)
	{
		if (auto downcast = std::dynamic_pointer_cast<Object>(value.leaf); downcast == nullptr)
			throw std::runtime_error("Mismatched type.");
		else
			return *downcast;
	}

	template <typename Type>
	void push(const std::string& key, const Type& data)
	{
		auto real = std::make_shared<Type>();
		*real = data;

		auto value = std::make_shared<Value>();
		value->leaf = real;

		this->pairs[key] = std::move(value);
	}

	Value& operator[](const char* key)
	{
		if (!this->exist(key))
			this->pairs[key] = std::make_shared<Value>();

		return *(this->pairs[key]);
	}

	template <typename CompositeType>
	CompositeType& get(const std::string& key)
	{
		if (!this->exist(key))
			throw std::runtime_error("Not exist key.");

		if constexpr (std::is_same_v<CompositeType, Array> ||
					  std::is_same_v<CompositeType, Object>) {
			if (auto downcast = std::dynamic_pointer_cast<CompositeType>(this->pairs[key]->leaf);
				downcast == nullptr)
				throw std::runtime_error(key + "Mismatched type.");
			else
				return *downcast;
		} else {
			static_assert(dependent_false<CompositeType>::value, "Only Composite type supported.");
		}
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

	void deserialize(const std::string& dumped) override
	{
		if (dumped.empty())
			throw std::invalid_argument("Dumped value cannot empty.");

		auto stripped = strip(trim(dumped), '{', '}');
		auto divided = split(stripped, ",");
		auto tokens = canonicalize(divided);
		for (const auto& token : tokens) {
			if (auto pos = token.find(":"); pos != std::string::npos) {
				auto lhs = token.substr(0, pos);
				auto rhs = token.substr(pos + 1);

				auto key = strip(trim(lhs), '"', '"');
				auto value = std::make_shared<Value>();

				value->deserialize(trim(rhs));
				this->pairs[key] = value;
			} else {
				throw std::runtime_error("Wrong format.");
			}
		}
	}

	std::size_t size() const noexcept
	{
		return this->pairs.size();
	}

	bool exist(const std::string& key) const noexcept
	{
		return this->pairs.find(key) != this->pairs.end();
	}

	std::unordered_map<std::string, std::shared_ptr<Value>> pairs;
};

} // namespace json
} // namespace vist
