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

#include "policy.hpp"
#include "table.hpp"

#include <vist/exception.hpp>
#include <vist/json.hpp>
#include <vist/logger.hpp>
#include <vist/policy/api.hpp>

extern "C" vist::table::DynamicTable* DynamicTableFactory()
{
	return new vist::table::SamplePolicyTable;
}

namespace vist {
namespace table {

void SamplePolicyTable::init()
{
	DynamicTable::Register("sample_policy", std::make_shared<SamplePolicyTable>());

	// Register policy to policy-manager
	using namespace policy;
	auto provider = std::make_shared<SampleProvider>("sample-provider");
	provider->add(std::make_shared<SampleIntPolicy>());
	provider->add(std::make_shared<SampleStrPolicy>());

	policy::API::Admin::AddProvider(std::move(provider));

	INFO(VIST_PLUGIN) << "Sample plugin loaded.";
}

TableColumns SamplePolicyTable::columns() const
{
	return {
		std::make_tuple("sample_int_policy", INTEGER_TYPE, ColumnOptions::DEFAULT),
		std::make_tuple("sample_str_policy", TEXT_TYPE, ColumnOptions::DEFAULT),
	};
}

TableRows SamplePolicyTable::generate(QueryContext&) try
{
	INFO(VIST) << "Select query about sample-policy table.";

	Row row;
	int intPolicy = vist::policy::API::Get("sample_int_policy");
	row["sample_int_policy"] = std::to_string(intPolicy);

	std::string strPolicy = vist::policy::API::Get("sample_str_policy");
	row["sample_str_policy"] = strPolicy;

	QueryData results;
	results.emplace_back(std::move(row));

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

QueryData SamplePolicyTable::update(QueryContext&, const PluginRequest& request) try
{
	INFO(VIST) << "Update query about sample-policy table.";
	if (request.count("json_values") == 0)
		throw std::runtime_error("Wrong request format. Not found json value.");

	DEBUG(VIST) << "Request values: " << request.at("json_values");
	json::Json document = json::Json::Parse(request.at("json_values"));
	json::Array values = document.get<json::Array>("values");
	if (values.size() != 2)
		throw std::runtime_error("Wrong request format.");

	policy::API::Admin::Set("sample_int_policy",
			policy::PolicyValue(static_cast<int>(values.at(0))));
	policy::API::Admin::Set("sample_str_policy",
			policy::PolicyValue(static_cast<std::string>(values.at(1))));

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
