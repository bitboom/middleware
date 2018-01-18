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

#include <string>
#include <vector>

namespace qxx {
namespace internal {

template<typename... Base>
class TableImpl {
public:
	std::vector<std::string> getColumnNames() const noexcept { return {}; }

	template<class Type>
	std::string getColumnName(Type type) const noexcept { return std::string(); }

	int size() const noexcept { return 0; }
};

template<typename Front, typename... Rest>
class TableImpl<Front, Rest...> : public TableImpl<Rest...> {
public:
	using Column = Front;

	explicit TableImpl(Front front, Rest ...rest) : Base(rest...), column(front) {}

	int size() const noexcept { return Base::size() + 1; }

	std::vector<std::string> getColumnNames(void) const noexcept {
		auto names = Base::getColumnNames();
		names.emplace_back(this->column.name);
		return names;
	}

	template<typename ColumnType>
	std::string getColumnName(ColumnType type) const noexcept {
		// [TO-DO] Do Not Cast.
		// [ALTER] std::is_same<F, typename T::field_type>{}
		// [PROBLEM] Cannot multi-table select..
		// [CRITICAL] (&Data::int == &Data2::int) is same
		if (reinterpret_cast<ColumnType>(column.type) == type)
			return column.name;

		return Base::template getColumnName<ColumnType>(type);
	}

private:
	using Base = TableImpl<Rest...>;

	Column column;
};

} // namespace internal
} // namespace qxx
