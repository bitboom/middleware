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

#include <vist/stringfy.hpp>

namespace vist {

Stringfy::Stringfy(int origin) noexcept :
	type(Type::Integer), buffer(convert(origin))
{
}

Stringfy::Stringfy(const std::string& origin) noexcept :
	type(Type::String), buffer(convert(origin))
{
}

Stringfy Stringfy::Restore(const std::string& dumped)
{
	auto parsed = Stringfy::Parse(dumped);
	switch (parsed.first) {
	case Type::Integer:
		return Stringfy(std::stoi(parsed.second));
	case Type::String:
		return Stringfy(parsed.second);
	case Type::None:
	default:
		THROW(ErrCode::LogicError) << "Invalid format.";
	}
}

Stringfy::Type Stringfy::GetType(const std::string& dumped)
{
	return Stringfy::Parse(dumped).first;
}

Stringfy::operator std::string() const
{
	auto parsed = Stringfy::Parse(this->buffer);
	if (parsed.first != Type::String)
		THROW(ErrCode::TypeUnsafed) << "Type is not safed.";

	return parsed.second;
}

Stringfy::operator int() const
{
	auto parsed = Stringfy::Parse(this->buffer);
	if (parsed.first != Type::Integer)
		THROW(ErrCode::TypeUnsafed) << "Type is not safed.";

	return std::stoi(parsed.second);
}

std::pair<Stringfy::Type, std::string> Stringfy::Parse(const std::string& dumped)
{
	std::string type = dumped.substr(0, dumped.find('/'));
	std::string data = dumped.substr(dumped.find('/') + 1);
	if (type.empty())
		THROW(ErrCode::LogicError) << "Invalid format.";

	return std::make_pair(static_cast<Type>(type.at(0)), data);
}

std::string Stringfy::dump() const noexcept
{
	return this->buffer;
}

} // namespace vist
