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

#include <algorithm>
#include <cctype>
#include <locale>
#include <regex>

namespace vist {

inline void ltrim(std::string& str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

inline void rtrim(std::string& str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), str.end());
}

inline std::string trim(const std::string& origin)
{
	std::string ret = origin;
	ltrim(ret);
	rtrim(ret);
	return ret;
}

inline std::vector<std::string> split(const std::string& origin, const std::string& expression)
{
	std::regex regex(expression);
	auto begin = std::sregex_token_iterator(origin.begin(), origin.end(), regex, -1);
	auto end = std::sregex_token_iterator();
	return std::vector<std::string>(begin, end);
}

inline std::string strip(const std::string& origin, char begin, char end)
{
	auto copied = origin;
	if (copied.size() < 2 || copied[0] != begin || copied[copied.size() - 1] != end)
		throw std::invalid_argument("Wrong format.");

	return copied.substr(1, copied.size() -2);
}

inline std::size_t count(const std::string& str, char ch)
{
	return std::count(str.begin(), str.end(), ch);
}

} // namespace vist
