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

inline void trim(std::string& str)
{
	ltrim(str);
	rtrim(str);
}

inline std::vector<std::string> split(const std::string origin, std::regex regex)
{
	auto begin = std::sregex_token_iterator(origin.begin(), origin.end(), regex, -1);
	auto end = std::sregex_token_iterator();
	return std::vector<std::string>(begin, end);
}

} // namespace vist
