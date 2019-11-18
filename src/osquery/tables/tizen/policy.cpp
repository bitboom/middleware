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

#include <string>
#include <memory>
#include <stdexcept>

#include <osquery/sql.h>
#include <osquery/tables.h>

#include <vist/policy/api.hpp>
#include <vist/logger.hpp>

namespace osquery {
namespace tables {

QueryData genPolicy(QueryContext& context) try {
	INFO(VIST) << "Select query about policy table.";

	QueryData results;
	if (context.constraints["name"].exists(EQUALS)) { /// where clause
		auto names = context.constraints["name"].getAll(EQUALS);
		for (const auto& name : names) {
			auto ret = vist::policy::API::Get(name);
			int value = ret;

			Row r;
			r["name"] = TEXT(name);
			r["value"] = TEXT(value);

			results.emplace_back(std::move(r));
		}
	} else { /// select *;
		auto policies = vist::policy::API::GetAll();
		for (auto& policy : policies) {
			int value = policy.second;

			Row r;
			r["name"] = TEXT(policy.first);
			r["value"] = TEXT(value);

			results.emplace_back(std::move(r));
		}
	}

	return results;
} catch (...) {
	ERROR(VIST) << "Failed to select query on policy.";
	Row r;
	return { r };
}

QueryData updatePolicy(QueryContext& context, const PluginRequest& request) try {
	INFO(VIST) << "Update query about policy table.";
	if (request.count("json_value_array") == 0)
		throw std::runtime_error("Wrong request format. Not found json value.");

	std::string str = request.at("json_value_array");
	rapidjson::Document document;
	document.Parse(str.c_str());
	if (document.HasParseError() || !document.IsArray())
		throw std::runtime_error("Cannot parse request.");

	if (document.Size() != 2)
		throw std::runtime_error("Wrong request format.");

	std::string name = document[0].GetString();
	int value = std::stoi(document[1].GetString());

	vist::policy::API::Admin::Set(name, vist::policy::PolicyValue(value));

	Row r;
	r["status"] = "success";
	return { r };
} catch (...) {
	ERROR(VIST) << "Failed to update query on policy.";
	Row r;
	return { r };
}

} // namespace tables
} // namespace osquery
