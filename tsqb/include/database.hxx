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
 * @file database.hxx
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Represent the database scheme of SQL
 */

#pragma once

#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>

#include "table-pack.hxx"
#include "tuple-helper.hxx"
#include "condition.hxx"
#include "expression.hxx"
#include "util.hxx"

namespace tsqb {

template<typename... Tables>
class Database {
public:
	using Self = Database<Tables...>;

	// TODO(Sangwan): Use Crud class
	template<typename... ColumnTypes>
	Self& select(ColumnTypes&&... cts);

	template<typename Expr>
	Self& where(Expr expr);

	template<typename Table>
	Self& join(condition::Join type = condition::Join::INNER);

	template<typename Expr>
	Self& on(Expr expr);

	operator std::string();

	std::string name;

private:
	using TablePackType = internal::TablePack<Tables...>;
	using ColumnNames = std::vector<std::string>;
	using TableNames = std::set<std::string>;

	explicit Database(const std::string& name, TablePackType&& tablePack);

	template<typename ...Ts>
	friend Database<Ts...> make_database(const std::string& name, Ts&& ...tables);

	template<typename Cs>
	std::set<std::string> getTableNames(Cs&& tuple);

	template<typename Cs>
	std::vector<std::string> getColumnNames(Cs&& tuple);

	struct GetTableNames {
		TablePackType tablePack;
		std::set<std::string> names;
		GetTableNames(const TablePackType& tablePack) : tablePack(tablePack) {}

		template <typename T>
		void operator()(T&& type)
		{
			auto column = make_column("anonymous", type);
			using TableType = typename decltype(column)::TableType;
			auto name = this->tablePack.getName(TableType());
			if (!name.empty())
				names.emplace(name);
		}
	};

	struct GetColumnNames {
		TablePackType tablePack;
		std::vector<std::string> names;

		GetColumnNames(const TablePackType& tablePack) : tablePack(tablePack) {}

		template <typename T>
		void operator()(T&& type)
		{
			auto column = make_column("anonymous", type);
			auto name = this->tablePack.getColumnName(std::move(column));
			if (!name.empty())
				names.emplace_back(name);
		}
	};

	template<typename L, typename R>
	std::string processWhere(condition::And<L,R>& expr);

	template<typename L, typename R>
	std::string processWhere(condition::Or<L,R>& expr);

	template<typename Expr>
	std::string processWhere(Expr expr);

	TablePackType tablePack;
	std::vector<std::string> cache;
};

template<typename ...Tables>
Database<Tables...> make_database(const std::string& name, Tables&& ...tables)
{
	auto tablePack = internal::TablePack<Tables...>(std::forward<Tables>(tables)...);
	return Database<Tables...>(name, std::move(tablePack));
}

template<typename ...Tables>
Database<Tables...>::Database(const std::string& name, TablePackType&& tablePack)
	: name(name), tablePack(std::move(tablePack)) {}

template<typename... Tables>
template<typename... ColumnTypes>
Database<Tables...>& Database<Tables...>::select(ColumnTypes&&... cts)
{
	this->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = this->getColumnNames(std::move(columnTuple));
	auto tableNames = this->getTableNames(std::move(columnTuple));

	std::stringstream ss;
	ss << "SELECT ";

	int i = 0;
	for (const auto& c : columnNames) {
		ss << c;

		if (i++ < columnNames.size() - 1)
			ss << ", ";
	}

	ss << " FROM ";

	i = 0;
	for (const auto& t : tableNames) {
		ss << t;

		if (i++ < tableNames.size() - 1)
			ss << ", ";
	}

	cache.emplace_back(ss.str());

	return *this;
}

template<typename... Tables>
template<typename Expr>
Database<Tables...>& Database<Tables...>::where(Expr expr)
{
	std::stringstream ss;
	ss << "WHERE " << this->processWhere(expr);

	this->cache.emplace_back(ss.str());

	return *this;
}

template<typename... Tables>
template<typename Table>
Database<Tables...>& Database<Tables...>::join(condition::Join type)
{
	std::stringstream ss;
	ss << condition::to_string(type) << " ";
	ss << "JOIN ";
	ss << this->tablePack.getName(Table());

	this->cache.emplace_back(ss.str());
	return *this;
}

template<typename... Tables>
template<typename Expr>
Database<Tables...>& Database<Tables...>::on(Expr expr)
{
	std::stringstream ss;
	ss << "ON ";

	auto lname = this->tablePack.getColumnName(std::move(expr.l));
	ss << lname << " ";

	ss << std::string(expr) << " ";

	auto rname = this->tablePack.getColumnName(std::move(expr.r));
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
template<typename Cs>
std::set<std::string> Database<Tables...>::getTableNames(Cs&& tuple)
{
	GetTableNames closure(this->tablePack);
	tuple_helper::for_each(std::forward<Cs>(tuple), closure);

	return closure.names;
}

template<typename... Tables>
template<typename Cs>
std::vector<std::string> Database<Tables...>::getColumnNames(Cs&& tuple)
{
	GetColumnNames closure(this->tablePack);
	tuple_helper::for_each(std::forward<Cs>(tuple), closure);

	return closure.names;
}

template<typename... Tables>
template<typename L, typename R>
std::string Database<Tables...>::processWhere(condition::And<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename... Tables>
template<typename L, typename R>
std::string Database<Tables...>::processWhere(condition::Or<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename... Tables>
template<typename Expr>
std::string Database<Tables...>::processWhere(Expr expr)
{
	std::stringstream ss;
	ss << this->tablePack.getColumnName(expr.l);
	ss << " " << std::string(expr) << " ?";

	return ss.str();
}

} // namespace tsqb
