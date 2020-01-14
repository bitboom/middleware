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

#include <string>
#include <memory>
#include <stdexcept>

#include <osquery/sql.h>
#include <osquery/tables.h>

#include <vist/policy/api.hpp>
#include <vist/exception.hpp>
#include <vist/logger.hpp>

namespace osquery {

namespace {

void getPolicy(Row& row, const std::string& name)
{
	int value = vist::policy::API::Get(name);
	row[name] = std::to_string(value);
}

} // anonymous namespace

namespace tables {

using namespace vist;

QueryData genBluetooth(QueryContext& context) try {
	INFO(VIST) << "Select query about policy table.";

	QueryData results;

	Row row;
	getPolicy(row, "bluetooth");
	getPolicy(row, "bluetooth-desktop-connectivity");
	getPolicy(row, "bluetooth-pairing");
	getPolicy(row, "bluetooth-tethering");

	results.emplace_back(std::move(row));

	return results;
} catch (const vist::Exception<ErrCode>& e) {
	ERROR(VIST) << "Failed to query: " << e.what();
	Row r;
	return { r };
} catch (...) {
	ERROR(VIST) << "Failed to query with unknown exception.";
	Row r;
	return { r };
}

} // namespace tables
} // namespace osquery
