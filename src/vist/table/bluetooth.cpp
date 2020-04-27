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

#include "bluetooth.hpp"

#include <vist/exception.hpp>
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

std::map<std::string, std::string> ALIAS = {
	{ "state", "bluetooth" },
	{ "desktopConnectivity", "bluetooth-desktop-connectivity" },
	{ "pairing", "bluetooth-pairing" },
	{ "tethering", "bluetooth-tethering"} };

void setPolicy(const std::string& name, int value)
{
	vist::policy::API::Admin::Set(name, vist::policy::PolicyValue(value));
}

} // anonymous namespace

void BluetoothTable::Init()
{
	auto tables = RegistryFactory::get().registry("table");
	tables->add("bluetooth", std::make_shared<BluetoothTable>());
}

TableColumns BluetoothTable::columns() const
{
	return {
		std::make_tuple("state", INTEGER_TYPE, ColumnOptions::DEFAULT),
		std::make_tuple("desktopConnectivity", INTEGER_TYPE, ColumnOptions::DEFAULT),
		std::make_tuple("pairing", INTEGER_TYPE, ColumnOptions::DEFAULT),
		std::make_tuple("tethering", INTEGER_TYPE, ColumnOptions::DEFAULT),
	};
}

TableRows BluetoothTable::generate(QueryContext&) try {
	INFO(VIST) << "Select query about bluetooth table.";

	QueryData results;

	Row row;

	for (const auto&[schemaName, policyName]: ALIAS) {
		int value = vist::policy::API::Get(policyName);
		row[schemaName] = std::to_string(value);
	}

	results.emplace_back(std::move(row));

	return osquery::tableRowsFromQueryData(std::move(results));
} catch (const vist::Exception<ErrCode>& e) {
	ERROR(VIST) << "Failed to query: " << e.what();
	Row r;
	return osquery::tableRowsFromQueryData({ r });
} catch (...) {
	ERROR(VIST) << "Failed to query with unknown exception.";
	Row r;
	return osquery::tableRowsFromQueryData({ r });
}

QueryData BluetoothTable::update(QueryContext&, const PluginRequest& request) try {
	INFO(VIST) << "Update query about bluetooth table.";
	if (request.count("json_value_array") == 0)
		throw std::runtime_error("Wrong request format. Not found json value.");

	std::string str = request.at("json_value_array");
	rapidjson::Document document;
	document.Parse(str.c_str());
	if (document.HasParseError() || !document.IsArray())
		throw std::runtime_error("Cannot parse request.");

	if (document.Size() != 4)
		throw std::runtime_error("Wrong request format.");

	/// TODO(Sangwan): Sync vtab schema with policy definition
	setPolicy("bluetooth", document[0].GetInt());
	setPolicy("bluetooth-desktop-connectivity", document[1].GetInt());
	setPolicy("bluetooth-pairing", document[2].GetInt());
	setPolicy("bluetooth-tethering", document[3].GetInt());

	Row r;
	r["status"] = "success";
	return { r };
} catch (const vist::Exception<ErrCode>& e) {
	ERROR(VIST) << "Failed to query: " << e.what();
	Row r;
	return { r };
} catch (...) {
	ERROR(VIST) << "Failed to query with unknown exception.";
	Row r;
	return { r };
}

} // namespace table
} // namespace vist
