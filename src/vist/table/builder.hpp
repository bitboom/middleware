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

#pragma once

#include <vist/exception.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>

#include <osquery/registry.h>
#include <osquery/sql/dynamic_table_row.h>
#include <osquery/tables.h>

using namespace osquery;

namespace vist {
namespace table {

template<class T> struct dependent_false : std::false_type {};

struct Builder {
	template <typename T>
	static void table(const std::string& name)
	{
		// Register virtual table to sqlite3
		auto tables = RegistryFactory::get().registry("table");
		tables->add(name, std::make_shared<T>());
	}

	template <typename T>
	static std::tuple<std::string, ColumnType, ColumnOptions> column(const std::string& name)
	{
		if constexpr(std::is_same_v<T, int>)
			return std::tuple(name, INTEGER_TYPE, ColumnOptions::DEFAULT);
		else if constexpr(std::is_same_v<T, std::string>)
			return std::tuple(name, TEXT_TYPE, ColumnOptions::DEFAULT);
		else
			static_assert(dependent_false<T>::value, "Not supported column type.");
	}
};

} // namespace table
} // namespace vist
