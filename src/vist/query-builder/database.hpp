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
#include "condition.hpp"
#include "crud.hpp"
#include "expression.hpp"
#include "util.hpp"

#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace vist {
namespace tsqb {

template<typename... Tables>
class Database : public Crud<Database<Tables...>> {
public:
	using Self = Database<Tables...>;

	explicit Database(const std::string& name, Tables ...tables) :
		name(name), tables(tables...) {}

	template<typename Table>
	Self& join(condition::Join type = condition::Join::INNER);

	template<typename Expr>
	Self& on(Expr expr);

	std::size_t size() const noexcept;

	operator std::string();

	std::string name;

public: // CRTP(Curiously Recurring Template Pattern) for CRUD
	template<typename... Cs>
	std::vector<std::string> getTableNames(Cs&& ...columns) const noexcept;
	template<typename Table>
	std::string getTableName(Table&& table) const noexcept;
	template<typename... Cs> 
	std::vector<std::string> getColumnNames(Cs&& ...columns) const noexcept;
	template<typename Column>
	std::string getColumnName(Column&& column) const noexcept;

	std::vector<std::string> cache;

private:
	std::tuple<Tables...> tables;
};

template<typename... Tables>
template<typename Table>
Database<Tables...>& Database<Tables...>::join(condition::Join type)
{
	std::stringstream ss;
	ss << condition::to_string(type) << " ";
	ss << "JOIN ";
	ss << this->getTableName(Table());

	this->cache.emplace_back(ss.str());
	return *this;
}

template<typename... Tables>
template<typename Expr>
Database<Tables...>& Database<Tables...>::on(Expr expr)
{
	std::stringstream ss;
	ss << "ON ";

	auto lname = this->getColumnName(std::move(expr.l.type));
	ss << lname << " ";

	ss << std::string(expr) << " ";

	auto rname = this->getColumnName(std::move(expr.r.type));
	ss << rname;

	this->cache.emplace_back(ss.str());
	return *this;
}

template<typename... Tables>
Database<Tables...>::operator std::string()
{
	std::stringstream ss;
	for (const auto& c : cache)
		ss << c << " ";

	this->cache.clear();
	return util::rtrim(ss.str());
}

template<typename... Tables>
template<typename... Cs>
std::vector<std::string> Database<Tables...>::getTableNames(Cs&& ...columns) const noexcept
{
	std::set<std::string> names;

	auto predicate = [this, &names](const auto& column) {
		using ColumnType = std::remove_reference_t<decltype(column)>;
		using TableType = typename ColumnType::Table;
		auto name = this->getTableName(TableType());
		if (!name.empty())
				names.emplace(name);
	};

	auto closure = [&predicate](const auto&... iter) {
		(predicate(iter), ...);
	};

	std::apply(closure, std::tuple(columns...));

	return std::vector<std::string>(names.begin(), names.end());
}

template<typename... Tables>
template<typename... Cs>
std::vector<std::string> Database<Tables...>::getColumnNames(Cs&& ...columns) const noexcept
{
	std::vector<std::string> names;
	auto predicate = [this, &names](const auto& column) {
		auto name = this->getColumnName(column);
		if (!name.empty())
			names.emplace_back(name);
	};

	auto closure = [&predicate](const auto&... iter) {
		(predicate(iter), ...);
	};

	std::apply(closure, std::tuple(columns...));

	return names;
}

template<typename... Tables>
template<typename Table>
std::string Database<Tables...>::getTableName(Table&& table) const noexcept
{
	std::string name;
	auto predicate = [&name, &table](const auto& type) {
		if (type.compare(table))
			name = type.name;
	};

	auto closure = [&predicate](const auto&... iter) {
		(predicate(iter), ...);
	};

	std::apply(closure, this->tables);

	return name;
}

template<typename... Tables>
template<typename Column>
std::string Database<Tables...>::getColumnName(Column&& column) const noexcept
{
	using ColumnType = std::remove_reference_t<decltype(column)>;
	using TableType = typename ColumnType::Table;
	TableType table;

	std::string name;
	auto predicate = [&name, &table, &column](const auto& iter) {
		if (iter.compare(table)) {
			auto cname = iter.getColumnName(column);
			name = iter.name + "." + cname;
		}
	};

	auto closure = [&predicate](const auto&... iter) {
		(predicate(iter), ...);
	};

	std::apply(closure, this->tables);

	return name;
}

template<typename... Tables>
std::size_t Database<Tables...>::size() const noexcept
{
	using TupleType = std::tuple<Tables...>;
	return std::tuple_size<TupleType>::value;
}

} // namespace tsqb
} // namespace vist
