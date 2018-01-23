/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "table.hxx"

#include <vector>
#include <set>
#include <string>

namespace qxx {
namespace internal {

template<typename... Base>
class DatabaseImpl {
public:
	template<typename Column>
	std::string getTableName(Column) const noexcept { return std::string(); }

	template<typename Column>
	std::string getColumnName(Column column) const noexcept { return std::string(); }
};

template<typename Front, typename... Rest>
class DatabaseImpl<Front, Rest...> : public DatabaseImpl<Rest...> {
public:
	using Table = Front;

	DatabaseImpl(Front front, Rest ...rest) : Base(rest...), table(front) {}

	Table table;

	template<typename Column>
	std::string getTableName(Column column) const noexcept
	{
		if (this->table.find(column))
			return this->table.name;

		return Base::template getTableName<Column>(column);
	}

	template<typename Column>
	std::string getColumnName(Column column) const noexcept
	{
		if (this->table.find(column)) {
			auto cname = this->table.getColumnName(column.type);
			return this->table.name + "." + cname;
		}

		return Base::template getColumnName<Column>(column);
	}

private:
	using Base = DatabaseImpl<Rest...>;
};

} // namespace internal
} // namespace qxx
