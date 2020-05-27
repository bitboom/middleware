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

#include <vist/string.hpp>

#include <string>
#include <vector>

namespace vist {
namespace json {

inline std::vector<std::string> canonicalize(std::vector<std::string>& tokens)
{
	std::vector<std::string> result;
	auto rearrange = [&](std::vector<std::string>::iterator & iter, char first, char last) {
		if ((*iter).find(first) == std::string::npos)
			return false;

		std::string origin = *iter;
		iter++;

		std::size_t lcount = 1, rcount = 0;
		while (iter != tokens.end()) {
			lcount += count(*iter, first);
			rcount += count(*iter, last);

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
		if (rearrange(iter, '{', '}') || rearrange(iter, '[', ']')) {
			if (iter == tokens.end())
				break;
			else
				continue;
		}

		result.emplace_back(std::move(*iter));
	}

	return result;
}

} // namespace json
} // namespace vist
