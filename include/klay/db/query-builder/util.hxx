/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

#pragma once

#include <string>
#include <algorithm>
#include <cctype>

namespace qxx {
namespace util {

std::string&& rtrim(std::string&& s)
{
	auto predicate = [](unsigned char c){ return !std::isspace(c); };
	auto base = std::find_if(s.rbegin(), s.rend(), predicate).base();
	s.erase(base, s.end());
	return std::move(s);
}

} // namespace util
} // namespace qxx
