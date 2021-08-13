/*
 *  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include "policy-admin.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>
#include <vist/policy/api.hpp>
#include <vist/table/builder.hpp>
#include <vist/table/parser.hpp>
#include <vist/table/util.hpp>

namespace vist {
namespace table {

namespace {

std::string removeAlias(std::string&& alias)
{
	auto pos = alias.find(";");
	auto token = alias.substr(0, pos);

	if (token == "name")
		return alias.erase(0, pos + 1);
	else
		return std::string();
}

} // anonymous namespace

void PolicyAdminTable::Init()
{
	Builder::table<PolicyAdminTable>("policy_admin");
}

TableColumns PolicyAdminTable::columns() const
{
	return {
		Builder::column<std::string>("name"),
		Builder::column<int>("activated"),
	};
}

QueryData PolicyAdminTable::select(QueryContext& context)
{
	TABLE_EXCEPTION_GUARD_START

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

	TABLE_EXCEPTION_GUARD_END
}

QueryData PolicyAdminTable::insert(QueryContext&, const PluginRequest& request)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Insert query about policy-admin table.";
	auto admin = Parser::column<std::string>(request, 0);

	DEBUG(VIST) << "Admin info [name]: " << admin;
	vist::policy::API::Admin::Enroll(admin);

	return success();

	TABLE_EXCEPTION_GUARD_END
}

QueryData PolicyAdminTable::delete_(QueryContext&, const PluginRequest& request)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Delete query about policy-admin table.";
	auto admin = removeAlias(Parser::column<std::string>(request, 0));

	DEBUG(VIST) << "Admin info [name]: " << admin;
	vist::policy::API::Admin::Disenroll(admin);

	return success();

	TABLE_EXCEPTION_GUARD_END
}

QueryData PolicyAdminTable::update(QueryContext&, const PluginRequest& request)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Update query about policy-admin table.";

	auto name = Parser::column<std::string>(request, 0);
	auto activated = Parser::column<int>(request, 1);

	vist::policy::API::Admin::Activate(name, activated);

	return success();

	TABLE_EXCEPTION_GUARD_END
}

} // namespace tables
} // namespace osquery
