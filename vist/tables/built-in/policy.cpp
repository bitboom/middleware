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

#include "policy.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>
#include <vist/policy/api.hpp>
#include <vist/table/builder.hpp>
#include <vist/table/parser.hpp>
#include <vist/table/util.hpp>

#include <memory>
#include <stdexcept>
#include <string>

namespace vist {
namespace table {

namespace {

Row convert(const std::string& name, const vist::policy::PolicyValue& value)
{
	Row r;
	r["name"] = name;
	r["value"] = value.dump();

	return r;
}

} // anonymous namespace

void PolicyTable::Init()
{
	Builder::table<PolicyTable>("policy");
}

TableColumns PolicyTable::columns() const
{
	return {
		Builder::column<std::string>("name"),
		Builder::column<std::string>("value")
	};
}

QueryData PolicyTable::select(QueryContext& context)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Select query about policy table.";

	QueryData results;
	if (context.constraints["name"].exists(EQUALS)) { /// where clause
		auto names = context.constraints["name"].getAll(EQUALS);
		for (const auto& name : names) {
			auto value = vist::policy::API::Get(name);
			auto row = convert(name, value);

			results.emplace_back(std::move(row));
		}
	} else { /// select *;
		auto policies = vist::policy::API::GetAll();
		for (auto& policy : policies) {
			auto row = convert(policy.first, policy.second);

			results.emplace_back(std::move(row));
		}
	}

	return results;

	TABLE_EXCEPTION_GUARD_END
}

QueryData PolicyTable::update(QueryContext&, const PluginRequest& request)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Update query about policy table.";

	auto name = Parser::column<std::string>(request, 0);
	auto dumpedValue = Parser::column<std::string>(request, 1);

	vist::policy::API::Admin::Set(name, vist::policy::PolicyValue(dumpedValue, true));

	return success();

	TABLE_EXCEPTION_GUARD_END
}

} // namespace table
} // namespace vist
