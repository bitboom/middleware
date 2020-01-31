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

#include "column.hpp"
#include "crud.hpp"
#include "type.hpp"
#include "util.hpp"

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace vist {
namespace tsqb {

template<typename... Columns>
class Table : public Crud<Table<Columns...>> {
public:
	/// Make first stuct type to table type
	using Type = typename std::tuple_element<0, std::tuple<Columns...>>::type::Table;

	explicit Table(const std::string& name, Columns&& ...columns) :
		name(name), columns(columns...) {}

	std::string getName(void) const noexcept;
	std::vector<std::string> getColumnNames(void) const noexcept;

	template<typename That>
	bool compare(const That& that) const noexcept;

	std::size_t size() const noexcept;

	operator std::string();

	const std::string name;

public: // CRTP(Curiously Recurring Template Pattern) for CRUD
	template<typename... Cs>
	std::vector<std::string> getTableNames(Cs&& ...coulmns) const noexcept;
	template<typename That>
	std::string getTableName(That&& type) const noexcept;
	template<typename... Cs>
	std::vector<std::string> getColumnNames(Cs&& ...columns) const noexcept;
	template<typename Column>
	std::string getColumnName(const Column& column) const noexcept;

	std::vector<std::string> cache;

private:
	std::tuple<Columns...> columns;
};

template<typename... Columns>
std::string Table<Columns...>::getName() const noexcept
{
	return this->name;
}

template<typename... Columns>
template<typename... Cs>
std::vector<std::string> Table<Columns...>::getTableNames(Cs&& ...) const noexcept
{
	return {this->name};
}

template<typename... Columns>
template<typename... Cs>
std::vector<std::string> Table<Columns...>::getColumnNames(Cs&& ...columns) const noexcept
{
	std::vector<std::string> names;
	auto predicate = [this, &names](const auto& type) {
		auto name = this->getColumnName(type);
		if (!name.empty())
			names.emplace_back(name);
	};

	auto closure = [&predicate](const auto&... iter) {
		(predicate(iter), ...);
	};

	std::apply(closure, std::tuple(columns...));

	return names;
}

template<typename... Columns>
template<typename That>
std::string Table<Columns...>::getTableName(That&&) const noexcept
{
	return this->name;
}

template<typename... Columns>
std::vector<std::string> Table<Columns...>::getColumnNames(void) const noexcept
{
	std::vector<std::string> names;
	auto closure = [&names](const auto&... iter) {
		(names.push_back(iter.name), ...);
	};

	std::apply(closure, this->columns);

	return names;
}

template<typename... Columns>
template<typename Column>
std::string Table<Columns...>::getColumnName(const Column& column) const noexcept
{
	std::string name;
	auto predicate = [&name, &column](const auto& iter) {
		if (type::cast_compare(column, iter.type)) 
			name = iter.name;
	};

	auto closure = [&predicate](const auto&... iter) {
		(predicate(iter), ...);
	};

	std::apply(closure, this->columns);

	return name;
}

template<typename... Columns>
template<typename That>
bool Table<Columns...>::compare(const That& that) const noexcept
{
	using This = Type;
	return type::compare(This(), that);
}

template<typename... Columns>
Table<Columns...>::operator std::string()
{
	std::stringstream ss;
	for (const auto& c : cache)
		ss << c << " ";

	this->cache.clear();
	return util::rtrim(ss.str());
}

template<typename... Columns>
std::size_t Table<Columns...>::size() const noexcept
{
	using TupleType = std::tuple<Columns...>;
	return std::tuple_size<TupleType>::value;
}

} // namespace tsqb
} // namespace vist
