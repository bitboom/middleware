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

#include <policyd/api.h>
#include <vist/common/audit/logger.h>

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

std::pair<std::string, int> parseAdmin(const std::string& request, bool insert = true)
{
	rapidjson::Document document;
	document.Parse(request.c_str());
	if (document.HasParseError() || !document.IsArray())
		throw std::runtime_error("Cannot parse request.");

	if (document.Size() != 2)
		throw std::runtime_error("Wrong request format.");

	if (insert) {
		return std::make_pair(document[0].GetString(), document[1].GetInt());
	} else { /// osquery transforms int to string in 'where clause' internally.
		std::string name, uid;
		std::string value = getValue(document[0].GetString(), "name");
		if (!value.empty()) {
			name = value;
			uid = getValue(document[1].GetString(), "uid");
		} else {
			name = getValue(document[1].GetString(), "name");
			uid = getValue(document[0].GetString(), "uid");
		}

		return std::make_pair(name, std::stoi(uid));
	}
}

} // anonymous namespace

namespace osquery {
namespace tables {

QueryData genPolicyAdmin(QueryContext& context) try {
	INFO(VIST, "Select query about policy-admin.");

	QueryData results;
	auto admins = policyd::API::Admin::GetAll();

	for (auto& admin : admins) {
		Row r;
		r["name"] = SQL_TEXT(admin.first);
		r["uid"] = INTEGER(admin.second);

		DEBUG(VIST, "Admin info [name]: " << r["name"] << ", [uid]: " << r["uid"]);
		results.emplace_back(std::move(r));
	}

	return results;
} catch (...) {
	ERROR(VIST, "Failed to select query on policy-admin.");
	Row r;
	return { r };
}

QueryData insertPolicyAdmin(QueryContext& context, const PluginRequest& request) try {
	INFO(VIST, "Insert query about policy-admin.");
	if (request.count("json_value_array") == 0)
		throw std::runtime_error("Wrong request format. Not found json value.");

	auto admin = parseAdmin(request.at("json_value_array"));
	DEBUG(VIST, "Admin info [name]: " << admin.first << ", [uid]: " << admin.second);
	policyd::API::Admin::Enroll(admin.first, admin.second);

	Row r;
	r["status"] = "success";
	return { r };
} catch (...) {
	ERROR(VIST, "Failed to insert query on policy-admin.");
	Row r;
	return { r };
}

QueryData deletePolicyAdmin(QueryContext& context, const PluginRequest& request) try {
	INFO(VIST, "Delete query about policy-admin.");
	if (request.count("json_value_array") == 0)
		throw std::runtime_error("Wrong request format. Not found json value.");

	auto admin = parseAdmin(request.at("json_value_array"), false);
	DEBUG(VIST, "Admin info [name]: " << admin.first << ", [uid]: " << admin.second);
	policyd::API::Admin::Disenroll(admin.first, admin.second);

	Row r;
	r["status"] = "success";
	return { r };
} catch (...) {
	ERROR(VIST, "Failed to delete query on policy-admin.");
	Row r;
	return { r };
}

} // namespace tables
} // namespace osquery
