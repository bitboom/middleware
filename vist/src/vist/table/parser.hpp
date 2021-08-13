/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/exception.hpp>
#include <vist/json.hpp>

#include <osquery/tables.h>

using namespace osquery;

namespace vist {
namespace table {

struct Parser {

	template <typename T>
	static auto column(const PluginRequest& request, std::size_t index) -> T
	{
		if (request.count("json_values") == 0)
			THROW(ErrCode::LogicError) << "Wrong request format. Not found json value.";

		json::Json document = json::Json::Parse(request.at("json_values"));
		json::Array values = document.get<json::Array>("values");
		if (values.size() < index)
			THROW(ErrCode::LogicError) << "Wrong index.";

		return static_cast<T>(values.at(index));
	}

};

} // namespace table
} // namespace vist
