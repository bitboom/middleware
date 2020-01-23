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
#include "type.hpp"

#include <string>
#include <tuple>
#include <vector>

namespace vist {
namespace tsqb {
namespace internal {

template<typename... Columns>
class ColumnPack final {
public:
	using TableType = typename std::tuple_element<0, std::tuple<Columns...>>::type::TableType;
	using TupleType = std::tuple<Columns...>;

	explicit ColumnPack(Columns&& ...columns) : columns(columns...)
	{
	}

	template<typename Column>
	std::string getName(const Column& column) const noexcept
	{
		std::string name;
		auto predicate = [&name, &column](const auto& iter) {
			if (type::cast_compare(column, iter.type)) 
				name = iter.name;
		};

		tuple_helper::for_each(this->columns, predicate);

		return name;
	}

	std::vector<std::string> getNames(void) const noexcept
	{
		std::vector<std::string> names;
		auto closure = [&names](const auto& iter) {
			names.push_back(iter.name);
		};

		tuple_helper::for_each(this->columns, closure);

		return names;
	}

	std::size_t size() const noexcept
	{
		return std::tuple_size<TupleType>::value;
	}

private:
	std::tuple<Columns...> columns;
};

} // namespace internal
} // namespace tsqb
} // namespace vist
