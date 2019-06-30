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

#include "crud.hxx"
#include "condition.hxx"
#include "expression.hxx"
#include "table-pack.hxx"
#include "tuple-helper.hxx"
#include "util.hxx"

#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>

namespace tsqb {

template<typename... Tables>
class Database : public Crud<Database<Tables...>> {
public:
	using Self = Database<Tables...>;

	virtual ~Database() = default;

	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	Database(Database&&) = default;
	Database& operator=(Database&&) = default;

	// Functions for Crud
	template<typename Cs>
	std::set<std::string> getTableNames(Cs&& tuple) const noexcept;
	template<typename Cs>
	std::vector<std::string> getColumnNames(Cs&& tuple) const noexcept;
	template<typename ColumnType>
	std::string getColumnName(ColumnType&& type) const noexcept;

	template<typename Table>
	Self& join(condition::Join type = condition::Join::INNER);

	template<typename Expr>
	Self& on(Expr expr);

	operator std::string();

	std::string name;
	std::vector<std::string> cache;

private:
	using TablePackType = internal::TablePack<Tables...>;
	using ColumnNames = std::vector<std::string>;
	using TableNames = std::set<std::string>;

	explicit Database(const std::string& name, TablePackType&& tablePack);

	template<typename ...Ts>
	friend Database<Ts...> make_database(const std::string& name, Ts&& ...tables);

	struct GetTableNames {
		const TablePackType& tablePack;
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
		const TablePackType& tablePack;
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

	TablePackType tablePack;
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
std::set<std::string> Database<Tables...>::getTableNames(Cs&& tuple) const noexcept
{
	GetTableNames closure(this->tablePack);
	tuple_helper::for_each(std::forward<Cs>(tuple), closure);

	return closure.names;
}

template<typename... Tables>
template<typename Cs>
std::vector<std::string> Database<Tables...>::getColumnNames(Cs&& tuple) const noexcept
{
	GetColumnNames closure(this->tablePack);
	tuple_helper::for_each(std::forward<Cs>(tuple), closure);

	return closure.names;
}

template<typename... Tables>
template<typename ColumnType>
std::string Database<Tables...>::getColumnName(ColumnType&& type) const noexcept
{
	auto column = make_column("anonymous", type);
	return this->tablePack.getColumnName(std::move(column));
}

} // namespace tsqb
