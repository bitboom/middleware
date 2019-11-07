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

#include "type.hpp"

#include <string>
#include <vector>

namespace vist {
namespace tsqb {
namespace internal {

template<typename... Base>
class ColumnPack {
public:
	virtual ~ColumnPack() = default;

	template<typename ColumnType>
	std::string getName(ColumnType&&) const noexcept { return std::string(); }
	std::vector<std::string> getNames(void) const noexcept { return {}; }

	int size() const noexcept { return 0; }
};

template<typename Front, typename... Rest>
class ColumnPack<Front, Rest...> : public ColumnPack<Rest...> {
public:
	using Column = Front;
	using TableType = typename Column::TableType;

	explicit ColumnPack(Front&& front, Rest&& ...rest);
	virtual ~ColumnPack() = default;

	ColumnPack(const ColumnPack&) = delete;
	ColumnPack& operator=(const ColumnPack&) = delete;

	ColumnPack(ColumnPack&&) = default;
	ColumnPack& operator=(ColumnPack&&) = default;

	template<typename ColumnType>
	std::string getName(ColumnType&& type) const noexcept;
	std::vector<std::string> getNames(void) const noexcept;

	int size() const noexcept { return Base::size() + 1; }

private:
	using Base = ColumnPack<Rest...>;

	Column column;
};

template<typename Front, typename... Rest>
ColumnPack<Front, Rest...>::ColumnPack(Front&& front, Rest&& ...rest) :
	Base(std::forward<Rest>(rest)...), column(front)
{
}

template<typename Front, typename... Rest>
std::vector<std::string> ColumnPack<Front, Rest...>::getNames(void) const noexcept
{
	auto names = Base::getNames();
	names.push_back(this->column.name);

	return std::move(names);
}

template<typename Front, typename... Rest>
template<typename ColumnType>
std::string ColumnPack<Front, Rest...>::getName(ColumnType&& type) const noexcept
{
	if (type::cast_compare(column.type, std::forward<ColumnType>(type)))
		return column.name;

	return Base::template getName<ColumnType>(std::forward<ColumnType>(type));
}

} // namespace internal
} // namespace tsqb
} // namespace vist
