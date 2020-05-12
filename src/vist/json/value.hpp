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

#include <stdexcept>
#include <string>
#include <type_traits>

namespace vist {
namespace json {

struct Int;
struct String;
struct Object;

template<class T> struct dependent_false : std::false_type {};

struct Value {
	virtual std::string serialize() const
	{
		if (leaf == nullptr)
			throw std::runtime_error("Leaf is not set yet.");

		return this->leaf->serialize();
	}

	template <typename Type>
	Value& operator=(const Type& data)
	{
		if constexpr (std::is_same_v<Type, int>)
			this->leaf = std::make_shared<Int>(data); 
		else if constexpr (std::is_same_v<typename std::decay<Type>::type, std::string> ||
						   std::is_same_v<typename std::decay<Type>::type, char*>)
			this->leaf = std::make_shared<String>(data); 
		else
			static_assert(dependent_false<Type>::value, "Not supported type.");

		return *this;
	}

	virtual operator int()
	{
		if (auto downcast = std::dynamic_pointer_cast<Int>(this->leaf); downcast == nullptr)
			throw std::runtime_error("Mismatched type.");

		return (*this->leaf).operator int();
	}

	virtual operator std::string()
	{
		if (auto downcast = std::dynamic_pointer_cast<String>(this->leaf); downcast == nullptr)
			throw std::runtime_error("Mismatched type.");

		return (*this->leaf).operator std::string();
	}

	std::shared_ptr<Value> leaf;
};

struct Int : public Value {
	explicit Int(int data) : data(data)
	{
	}

	std::string serialize() const override
	{
		return std::to_string(data);
	}

	operator int() override
	{
		return data;
	}

	int data = 0;
};

struct String : public Value {
	explicit String(const std::string& data) : data(data)
	{
	}

	std::string serialize() const override
	{
		return "\"" + data + "\"";
	}

	operator std::string() override
	{
		return data;
	}

	std::string data;
};

} // namespace json
} // namespace vist
