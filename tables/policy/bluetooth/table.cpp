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

void setPolicy(const std::string& name, int value)
{
	vist::policy::API::Admin::Set(name, vist::policy::PolicyValue(value));
}

std::string getPolicy(const std::string& name)
{
	int value = vist::policy::API::Get(name);
	return std::to_string(value);
}

} // anonymous namespace

void BluetoothTable::init()
{
	Builder::table<BluetoothTable>("bluetooth");

	using namespace vist::policy;
	auto provider = std::make_shared<BluetoothProvider>("bluetooth");
	provider->add(std::make_shared<BluetoothProvider::State>());
	provider->add(std::make_shared<BluetoothProvider::DesktopConnectivity>());
	provider->add(std::make_shared<BluetoothProvider::Pairing>());
	provider->add(std::make_shared<BluetoothProvider::Tethering>());

	policy::API::Admin::AddProvider(provider);

	INFO(VIST_PLUGIN) << "Bluetooth plugin loaded.";
}

TableColumns BluetoothTable::columns() const
{
	// Define Virtual table's schema (column)
	using namespace schema;
	return {
		Builder::column<int>(Bluetooth::State.name),
		Builder::column<int>(Bluetooth::DesktopConnectivity.name),
		Builder::column<int>(Bluetooth::Pairing.name),
		Builder::column<int>(Bluetooth::Tethering.name)
	};
}

QueryData BluetoothTable::select(QueryContext&)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Select query about bluetooth table.";

	using namespace schema;
	using namespace policy;
	// Policy name format: bluetooth-xxx
	// Virtual table column name formant: xxx
	Row row;
	row[Bluetooth::State.name] = getPolicy(GetPolicyName(Bluetooth::State));
	row[Bluetooth::DesktopConnectivity.name] =
		getPolicy(GetPolicyName(Bluetooth::DesktopConnectivity));
	row[Bluetooth::Pairing.name] = getPolicy(GetPolicyName(Bluetooth::Pairing));
	row[Bluetooth::Tethering.name] = getPolicy(GetPolicyName(Bluetooth::Tethering));

	QueryData results;
	results.emplace_back(std::move(row));

	return results;

	TABLE_EXCEPTION_GUARD_END
}

QueryData BluetoothTable::update(QueryContext&, const PluginRequest& request)
{
	TABLE_EXCEPTION_GUARD_START

	INFO(VIST) << "Update query about bluetooth table.";

	using namespace schema;
	using namespace policy;
	setPolicy(GetPolicyName(Bluetooth::State), Parser::column<int>(request, 0));
	setPolicy(GetPolicyName(Bluetooth::DesktopConnectivity), Parser::column<int>(request, 1));
	setPolicy(GetPolicyName(Bluetooth::Pairing), Parser::column<int>(request, 2));
	setPolicy(GetPolicyName(Bluetooth::Tethering), Parser::column<int>(request, 3));

	return success();

	TABLE_EXCEPTION_GUARD_END
}

} // namespace table
} // namespace vist
