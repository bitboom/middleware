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
#include <vist/json.hpp>
#include <vist/logger.hpp>
#include <vist/policy/api.hpp>

#include <osquery/registry.h>
#include <osquery/sql/dynamic_table_row.h>

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
	auto tables = RegistryFactory::get().registry("table");
	tables->add("policy", std::make_shared<PolicyTable>());
}

TableColumns PolicyTable::columns() const
{
	return {
		std::make_tuple("name", TEXT_TYPE, ColumnOptions::DEFAULT),
		std::make_tuple("value", TEXT_TYPE, ColumnOptions::DEFAULT),
	};
}

TableRows PolicyTable::generate(QueryContext& context) try
{
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

	return osquery::tableRowsFromQueryData(std::move(results));
} catch (const vist::Exception<ErrCode>& e)
{
	ERROR(VIST) << "Failed to query: " << e.what();
	Row r;
	return osquery::tableRowsFromQueryData({ r });
} catch (...)
{
	ERROR(VIST) << "Failed to query with unknown exception.";
	Row r;
	return osquery::tableRowsFromQueryData({ r });
}

QueryData PolicyTable::update(QueryContext&, const PluginRequest& request) try
{
	INFO(VIST) << "Update query about policy table.";
	if (request.count("json_values") == 0)
		throw std::runtime_error("Wrong request format. Not found json value.");

	DEBUG(VIST) << "Request values: " << request.at("json_values");
	json::Json document = json::Json::Parse(request.at("json_values"));
	json::Array values = document.get<json::Array>("values");
	if (values.size() != 2)
		throw std::runtime_error("Wrong request format.");

	std::string name = static_cast<std::string>(values.at(0));
	std::string dumpedValue = static_cast<std::string>(values.at(1));

	vist::policy::API::Admin::Set(name, vist::policy::PolicyValue(dumpedValue, true));

	Row r;
	r["status"] = "success";
	return { r };
} catch (const vist::Exception<ErrCode>& e)
{
	ERROR(VIST) << "Failed to query: " << e.what();
	Row r;
	return { r };
} catch (...)
{
	ERROR(VIST) << "Failed to query with unknown exception.";
	Row r;
	return { r };
}

} // namespace table
} // namespace vist
