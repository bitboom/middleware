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
 *  See the License for the language governing permissions and
 *  limitations under the License
 */
/*
 * @brief  Convert type to string and vice versa.
 * @usage
 *  /// type to string
 *  std::string dumped = Strinfy::Dump(1000);
 *  EXPECT_EQ(dumped, "I/1000");
 *
 *  /// dumped string to origin type
 *  int origin = Stringify::Restore(dumped);
 *  EXPECT_EQ(origin, "1000");
 */

#pragma once

#include <vist/exception.hpp>

#include <string>

namespace vist {

class Stringify final {
public:
	enum class Type : char {
		Integer = 'I',
		String = 'S',
		None = 'N'
	};

	template <typename T>
	static std::string Dump(T origin);
	static Stringify Restore(const std::string& dumped);

	static Type GetType(const std::string& dumped);

	operator std::string() const;
	operator int() const;

private:
	explicit Stringify(int origin) noexcept;
	explicit Stringify(const std::string& origin) noexcept;

	static std::pair<Type, std::string> Parse(const std::string& dumped);

	template <typename T>
	std::string convert(const T& origin) const noexcept;

	std::string dump() const noexcept;

	Type type = Type::None;
	std::string buffer;
};

template <typename T>
std::string Stringify::Dump(T origin)
{
	return Stringify(origin).dump();
}

template <typename T>
std::string Stringify::convert(const T& origin) const noexcept
{
	std::stringstream ss;
	ss << static_cast<char>(this->type) << '/' << origin;

	return ss.str();
}

} // namespace vist
