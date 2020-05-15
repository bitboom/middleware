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

struct Bool;
struct Int;
struct String;
struct Null;

struct Array;
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
	void convert()
	{
		this->leaf = std::make_shared<Type>();
	}

	virtual void deserialize(const std::string& dumped)
	{
		if (dumped.empty())
			throw std::invalid_argument("Dumped value cannot empty.");

		switch (dumped[0]) {
			case '{' : this->convert<Object>(); break;
			case '[' : this->convert<Array>(); break;
			case '"': this->convert<String>(); break;
			case 'n': this->convert<Null>(); break;
			case 't': // fall through
			case 'f' : this->convert<Bool>(); break;
			default : this->convert<Int>();
		}

		this->leaf->deserialize(dumped);
	}

	template <typename Type>
	Value& operator=(const Type& data)
	{
		if constexpr (std::is_same_v<Type, int>)
			this->leaf = std::make_shared<Int>(data);
		else if constexpr (std::is_same_v<typename std::decay<Type>::type, std::string> ||
						   std::is_same_v<typename std::decay<Type>::type, char*>)
			this->leaf = std::make_shared<String>(data);
		else if constexpr (std::is_same_v<Type, bool>)
			this->leaf = std::make_shared<Bool>(data);
		else if constexpr (std::is_same_v<Type, std::nullptr_t>)
			this->leaf = std::make_shared<Null>();
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

	virtual operator bool()
	{
		if (auto downcast = std::dynamic_pointer_cast<Bool>(this->leaf); downcast == nullptr)
			throw std::runtime_error("Mismatched type.");

		return (*this->leaf).operator bool();
	}

	template <typename Type>
	bool is()
	{
		auto downcast = std::dynamic_pointer_cast<Type>(this->leaf);
		return downcast != nullptr;
	}

	std::shared_ptr<Value> leaf;
};

struct Int : public Value {
	explicit Int() {}
	explicit Int(int data) : data(data) {}

	std::string serialize() const override
	{
		return std::to_string(data);
	}

	void deserialize(const std::string& dumped) override
	{
		this->data = std::stoi(dumped);
	}

	operator int() override
	{
		return data;
	}

	int data = 0;
};

struct String : public Value {
	explicit String() {}
	explicit String(const std::string& data) : data(data) {}

	std::string serialize() const override
	{
		return "\"" + this->data + "\"";
	}

	void deserialize(const std::string& dumped) override
	{
		if (dumped.empty())
			throw std::invalid_argument("Dumped value cannot empty.");

		if (dumped[0] != '"' || dumped[dumped.size() - 1] != '"')
			throw std::invalid_argument("Wrong format.");

		this->data = dumped.substr(1, dumped.size() -2);
	}

	operator std::string() override
	{
		return this->data;
	}

	std::string data;
};

struct Bool : public Value {
	explicit Bool() {}
	explicit Bool(bool data) : data(data) {}

	std::string serialize() const override
	{
		return this->data ? "true" : "false";
	}

	void deserialize(const std::string& dumped) override
	{
		if (dumped == "true")
			this->data = true;
		else if (dumped == "false")
			this->data = false;
		else
			throw std::invalid_argument("Wrong format.");
	}

	operator bool() override
	{
		return this->data;
	}

	bool data = false;
};

struct Null : public Value {
	explicit Null() {}

	std::string serialize() const override
	{
		return "null";
	}

	void deserialize(const std::string& dumped) override
	{
		if (dumped != "null")
			throw std::invalid_argument("Wrong format.");
	}
};

} // namespace json
} // namespace vist
