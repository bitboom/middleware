/*
 *  Copyright (c) 2017-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include "tuple-helper.hpp"

#include <set>
#include <string>
#include <tuple>
#include <vector>

namespace vist {
namespace tsqb {
namespace internal {

template<typename... Tables>
class TablePack final {
public:
	using TupleType = std::tuple<Tables...>;

	explicit TablePack(Tables ...tables) : tables(tables...)
	{
	}

	template<typename TableType>
	std::string getName(TableType&& table) const noexcept
	{
		std::string name;
		auto predicate = [&name, &table](const auto& iter) {
			if (iter.compare(table))
				name = iter.name;
		};

		tuple_helper::for_each(this->tables, predicate);

		return name;
	}

	template<typename ColumnType>
	std::string getColumnName(ColumnType&& column) const noexcept
	{
		using TableType = typename ColumnType::Table;

		std::string name;
		TableType table;
		auto predicate = [&name, &table, column](const auto& iter) {
			if (iter.compare(table)) {
				auto cname = iter.getColumnName(column.type);
				name = iter.name + "." + cname;
			}
		};

		tuple_helper::for_each(this->tables, predicate);

		return name;
	}

private:
	std::tuple<Tables...> tables;
};

} // namespace internal
} // namespace tsqb
} // namespace vist
