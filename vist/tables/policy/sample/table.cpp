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
#include <vist/logger.hpp>
#include <vist/policy/api.hpp>
#include <vist/table/builder.hpp>
#include <vist/table/parser.hpp>
#include <vist/table/util.hpp>

extern "C" vist::table::DynamicTable* DynamicTableFactory()
{
	return new vist::table::SamplePolicyTable;
}

namespace vist {
namespace table {

void SamplePolicyTable::init()
{
	Builder::table<SamplePolicyTable>("sample_policy");

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
		Builder::column<int>("sample_int_policy"),
		Builder::column<std::string>("sample_str_policy")
	};
}

QueryData SamplePolicyTable::select(QueryContext&)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Select query about sample-policy table.";

	Row row;
	int intPolicy = vist::policy::API::Get("sample_int_policy");
	row["sample_int_policy"] = std::to_string(intPolicy);

	std::string strPolicy = vist::policy::API::Get("sample_str_policy");
	row["sample_str_policy"] = strPolicy;

	QueryData results;
	results.emplace_back(std::move(row));

	return results;

	TABLE_EXCEPTION_GUARD_END
}

QueryData SamplePolicyTable::update(QueryContext&, const PluginRequest& request)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Update query about sample-policy table.";

	auto intPolicy = Parser::column<int>(request, 0);
	auto strPolicy = Parser::column<std::string>(request, 1);

	policy::API::Admin::Set("sample_int_policy", policy::PolicyValue(intPolicy));
	policy::API::Admin::Set("sample_str_policy", policy::PolicyValue(strPolicy));

	return success();

	TABLE_EXCEPTION_GUARD_END
}

} // namespace table
} // namespace vist
