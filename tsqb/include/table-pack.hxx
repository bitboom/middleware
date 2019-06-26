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
/*
 * @file table-pack.hxx
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Tie diffirent types of tables
 */

#pragma once

#include <vector>
#include <set>
#include <string>

namespace tsqb {
namespace internal {

template<typename... Base>
class TablePack {
public:
	template<typename TableType>
	std::string getName(TableType&&) const noexcept { return std::string(); }

	template<typename ColumnType>
	std::string getColumnName(ColumnType&&) const noexcept { return std::string(); }
};

template<typename Front, typename... Rest>
class TablePack<Front, Rest...> : public TablePack<Rest...> {
public:
	using Table = Front;

	TablePack(Front&& front, Rest&& ...rest);

	template<typename TableType>
	std::string getName(TableType&& table) const noexcept;

	template<typename ColumnType>
	std::string getColumnName(ColumnType&& column) const noexcept;

private:
	using Base = TablePack<Rest...>;

	Table table;
};

template<typename Front, typename... Rest>
TablePack<Front, Rest...>::TablePack(Front&& front, Rest&& ...rest) :
	Base(std::forward<Rest>(rest)...), table(front)
{
}

template<typename Front, typename... Rest>
template<typename TableType>
std::string TablePack<Front, Rest...>::getName(TableType&& table) const noexcept
{
	if (this->table.compare(table))
		return this->table.name;

	return Base::template getName<TableType>(std::forward<TableType>(table));
}

template<typename Front, typename... Rest>
template<typename ColumnType>
std::string TablePack<Front, Rest...>::getColumnName(ColumnType&& column) const noexcept
{
	using DecayColumnType = typename std::decay<ColumnType>::type;
	using DecayTableType = typename DecayColumnType::TableType;
	if (this->table.compare(DecayTableType())) {
		auto cname = this->table.getColumnName(column.type);
		return this->table.name + "." + cname;
	}

	return Base::template getColumnName<ColumnType>(std::forward<ColumnType>(column));
}

} // namespace internal
} // namespace tsqb
