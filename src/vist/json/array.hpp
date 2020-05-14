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
#include <vist/string.hpp>

#include <string>
#include <vector>

namespace vist {
namespace json {

struct Array : public Value {
	template <typename Type>
	void push(const Type& data)
	{
		auto value = std::make_shared<Value>();
		*value = data;
		this->buffer.emplace_back(std::move(value));
	}

	Value& at(std::size_t index)
	{
		if (index > this->size())
			throw std::invalid_argument("Wrong index.");

		return *(this->buffer[index]);
	}

	Value& operator[](std::size_t index)
	{
		if (index > this->size())
			throw std::invalid_argument("Wrong index.");

		return *(this->buffer[index]);
	}

	std::string serialize() const override
	{
		std::stringstream ss;
		ss << "[ ";

		std::size_t i = 0;
		for (const auto& value : buffer) {
			ss << value->serialize();

			if (i++ < buffer.size() - 1)
				ss << ",";

			ss << " ";
		}
		ss << "]";

		return ss.str();
	}

	void deserialize(const std::string& dumped) override
	{
		if (dumped.empty())
			throw std::invalid_argument("Dumped value cannot empty.");

		auto stripped = strip(dumped, '[', ']');
		auto tokens = split(trim(stripped), ",");
		for (const auto& token : tokens) {
			auto value = std::make_shared<Value>();
			value->deserialize(trim(token));
			this->buffer.emplace_back(std::move(value));
		}
	}

	std::size_t size() const noexcept
	{
		return this->buffer.size();
	}

	std::vector<std::shared_ptr<Value>> buffer;
};

} // namespace json
} // namespace vist
