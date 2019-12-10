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

namespace {

std::string getValue(std::string&& alias, const std::string& key)
{
	auto pos = alias.find(";");
	auto token = alias.substr(0, pos);

	if (token == key)
		return alias.erase(0, pos + 1);
	else
		return std::string();
}

std::string parseAdmin(const std::string& request, bool insert = true)
{
	rapidjson::Document document;
	document.Parse(request.c_str());
	if (document.HasParseError() || !document.IsArray())
		throw std::runtime_error("Cannot parse request.");

	if (document.Size() != 1)
		throw std::runtime_error("Wrong request format.");

	if (insert)
		return std::string(document[0].GetString());
	else
		return getValue(document[0].GetString(), "name");
}

} // anonymous namespace

namespace osquery {
namespace tables {

QueryData genPolicyAdmin(QueryContext& context) try {
	INFO(VIST) << "Select query about policy-admin table.";

	QueryData results;
	auto admins = vist::policy::API::Admin::GetAll();

	for (auto& admin : admins) {
		if (context.constraints["name"].exists(EQUALS)) { /// where clause
			auto names = context.constraints["name"].getAll(EQUALS);
			for (const auto& name : names) {
				if (name == admin.first) {
					Row row;
					row["name"] = admin.first;
					row["activated"] = std::to_string(admin.second);

					DEBUG(VIST) << "Admin info [name]: " << row["name"]
								<< ", [activated]:" << row["activated"];

					results.emplace_back(std::move(row));
				}
			}
		} else { /// select *;
			Row row;
			row["name"] = admin.first;
			row["activated"] = std::to_string(admin.second);

			DEBUG(VIST) << "Admin info [name]: " << row["name"]
						<< ", [activated]:" << row["activated"];
			results.emplace_back(std::move(row));
		}
	}

	return results;
} catch (...) {
	ERROR(VIST) << "Failed to select query on policy-admin.";
	Row r;
	return { r };
}

QueryData insertPolicyAdmin(QueryContext& context, const PluginRequest& request) try {
	INFO(VIST) << "Insert query about policy-admin table.";
	if (request.count("json_value_array") == 0)
		throw std::runtime_error("Wrong request format. Not found json value.");

	auto admin = parseAdmin(request.at("json_value_array"));
	DEBUG(VIST) << "Admin info [name]: " << admin;
	vist::policy::API::Admin::Enroll(admin);

	Row r;
	r["status"] = "success";
	return { r };
} catch (...) {
	ERROR(VIST) << "Failed to insert query on policy-admin.";
	Row r;
	return { r };
}

QueryData deletePolicyAdmin(QueryContext& context, const PluginRequest& request) try {
	INFO(VIST) << "Delete query about policy-admin table.";
	if (request.count("json_value_array") == 0)
		throw std::runtime_error("Wrong request format. Not found json value.");

	auto admin = parseAdmin(request.at("json_value_array"), false);
	DEBUG(VIST) << "Admin info [name]: " << admin;
	vist::policy::API::Admin::Disenroll(admin);

	Row r;
	r["status"] = "success";
	return { r };
} catch (...) {
	ERROR(VIST) << "Failed to delete query on policy-admin.";
	Row r;
	return { r };
}

QueryData updatePolicyAdmin(QueryContext& context, const PluginRequest& request) try {
	INFO(VIST) << "Update query about policy-admin table.";
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
	int activated = document[1].GetInt();

	vist::policy::API::Admin::Activate(name, activated);

	Row r;
	r["status"] = "success";
	return { r };
} catch (...) {
	ERROR(VIST) << "Failed to insert query on policy-admin.";
	Row r;
	return { r };
}

} // namespace tables
} // namespace osquery
