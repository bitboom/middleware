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
#include <unordered_map>

namespace vist {
namespace json {

struct Object : public Value {
	template <typename Type>
	void push(const std::string& key, const Type& data)
	{
		auto value = std::make_shared<Value>();
		*value = data;
		this->pairs[key] = std::move(value);
	}

	Value& operator[](const char* key)
	{
		if (!this->exist(key))
			this->pairs[key] = std::make_shared<Value>();

		return *(this->pairs[key]);
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

		auto stripped = strip(dumped, '{', '}');
		auto divided = split(trim(stripped), ",");
		auto tokens = this->canonicalize(divided);
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

	std::vector<std::string> canonicalize(std::vector<std::string>& tokens)
	{
		std::vector<std::string> result;
		auto rearrange = [&](std::vector<std::string>::iterator& iter, char first, char last) {
			if ((*iter).find(first) == std::string::npos)
				return false;

			std::string origin = *iter;
			iter++;

			std::size_t lcount = 1, rcount = 0;
			while (iter != tokens.end()) {
				if ((*iter).find(first) != std::string::npos)
					lcount++;
				else if ((*iter).find(last) != std::string::npos)
					rcount++;

				origin += ", " + *iter;

				if (lcount == rcount)
					break;
				else
					iter++;
			}
			result.emplace_back(std::move(origin));

			return true;
		};

		for (auto iter = tokens.begin() ; iter != tokens.end(); iter++) {
			if (rearrange(iter, '{', '}') || rearrange(iter, '[', ']'))
				continue;

			result.emplace_back(std::move(*iter));
		}

		return result;
	}
};

} // namespace json
} // namespace vist
