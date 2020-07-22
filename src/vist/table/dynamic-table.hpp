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

#include <memory>

#include <osquery/registry.h>
#include <osquery/sql/dynamic_table_row.h>
#include <osquery/tables.h>

using namespace osquery;

namespace vist {
namespace table {

class DynamicTable : public TablePlugin {
public:
	using FactoryType = DynamicTable* (*)();

	virtual void init() = 0;

	template <typename T>
	static void Register(const std::string& name, std::shared_ptr<T>&& table)
	{
		// Register virtual table to sqlite3
		auto tables = RegistryFactory::get().registry("table");
		tables->add(name, std::move(table));
	}
};

} // namespace table
} // namespace vist
