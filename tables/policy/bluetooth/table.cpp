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
	return new vist::table::BluetoothTable;
}

namespace vist {
namespace table {

namespace {

std::map<std::string, std::string> ALIAS = {
	{ "state", "bluetooth" },
	{ "desktopConnectivity", "bluetooth-desktop-connectivity" },
	{ "pairing", "bluetooth-pairing" },
	{ "tethering", "bluetooth-tethering"}
};

void setPolicy(const std::string& name, int value)
{
	vist::policy::API::Admin::Set(name, vist::policy::PolicyValue(value));
}

} // anonymous namespace

void BluetoothTable::init()
{
	Builder::table<BluetoothTable>("bluetooth");

	auto provider = std::make_shared<policy::Bluetooth>("bluetooth");
	provider->add(std::make_shared<policy::BluetoothState>());
	provider->add(std::make_shared<policy::DesktopConnectivity>());
	provider->add(std::make_shared<policy::Pairing>());
	provider->add(std::make_shared<policy::Tethering>());

	policy::API::Admin::AddProvider(provider);

	INFO(VIST_PLUGIN) << "Bluetooth plugin loaded.";
}

TableColumns BluetoothTable::columns() const
{
	return {
		Builder::column<int>("state"),
		Builder::column<int>("desktopConnectivity"),
		Builder::column<int>("pairing"),
		Builder::column<int>("tethering")
	};
}

QueryData BluetoothTable::select(QueryContext&)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Select query about bluetooth table.";

	Row row;
	for (const auto& [schemaName, policyName] : ALIAS) {
		int value = vist::policy::API::Get(policyName);
		row[schemaName] = std::to_string(value);
	}

	QueryData results;
	results.emplace_back(std::move(row));

	return results;

	TABLE_EXCEPTION_GUARD_END
}

QueryData BluetoothTable::update(QueryContext&, const PluginRequest& request)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Update query about bluetooth table.";

	/// TODO(Sangwan): Sync vtab schema with policy definition
	setPolicy("bluetooth", Parser::column<int>(request, 0));
	setPolicy("bluetooth-desktop-connectivity", Parser::column<int>(request, 1));
	setPolicy("bluetooth-pairing", Parser::column<int>(request, 2));
	setPolicy("bluetooth-tethering", Parser::column<int>(request, 3));

	return success();

	TABLE_EXCEPTION_GUARD_END
}

} // namespace table
} // namespace vist
