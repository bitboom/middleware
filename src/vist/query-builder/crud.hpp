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
#include "expression.hpp"

#include <string>
#include <sstream>
#include <tuple>

namespace vist {
namespace tsqb {

template<typename T>
class Crud {
public:
	template<typename... ColumnTypes>
	T& select(ColumnTypes&&... cts);

	template<typename TableType>
	T& selectAll(void);

	T& selectAll(void);

	template<typename... AssginExpressions>
	T& update(AssginExpressions&&... expression);

	template<typename... ColumnTypes>
	T& insert(ColumnTypes&&... cts);

	template<typename... ColumnTypes>
	T& remove(ColumnTypes&&... cts);

	template<typename Expr>
	T& where(Expr expr);

private:
	template<typename L, typename R>
	std::string processWhere(And<L,R>& expr);

	template<typename L, typename R>
	std::string processWhere(Or<L,R>& expr);

	template<typename Expr>
	std::string processWhere(const Expr& expr);
};

template<typename T>
template<typename... ColumnTypes>
T& Crud<T>::select(ColumnTypes&&... cts)
{
	static_cast<T*>(this)->cache.clear();

	auto columnNames = static_cast<T*>(this)->getColumnNames(std::forward<ColumnTypes>(cts)...);
	auto tableNames = static_cast<T*>(this)->getTableNames(std::forward<ColumnTypes>(cts)...);

	std::stringstream ss;
	ss << "SELECT ";

	std::size_t i = 0;
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

	static_cast<T*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<T*>(this));
}

template<typename T>
T& Crud<T>::selectAll(void)
{
	static_cast<T*>(this)->cache.clear();

	std::stringstream ss;
	ss << "SELECT * FROM " << static_cast<T*>(this)->name;

	static_cast<T*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<T*>(this));
}

template<typename T>
template<typename TableType>
T& Crud<T>::selectAll(void)
{
	static_cast<T*>(this)->cache.clear();

	std::stringstream ss;
	auto tableName = static_cast<T*>(this)->getTableName(TableType());
	ss << "SELECT * FROM " << tableName;

	static_cast<T*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<T*>(this));
}

template<typename T>
template<typename... AssginExpressions>
T& Crud<T>::update(AssginExpressions&&... expression)
{
	static_cast<T*>(this)->cache.clear();

	std::stringstream ss;
	ss << "UPDATE " << static_cast<T*>(this)->name << " ";
	ss << "SET ";

	auto closure = [&ss, this](const auto&... iter) {
		(static_cast<void>( /// Make fold expression possible
			(ss << static_cast<T*>(this)->getColumnName(iter.l) /// Column name
				<< " " << static_cast<std::string>(iter) << " " /// Assign operator
				<< iter.r << ", ") /// Value
		), ...); /// Process fold expression
	};
	std::apply(closure, std::tuple(expression...));

	std::string raw = ss.str();
	if (raw.size() > 2)
		raw.erase(raw.end() - 2, raw.end()); /// Remove last ", "

	static_cast<T*>(this)->cache.emplace_back(std::move(raw));

	return *(static_cast<T*>(this));
}

template<typename T>
template<typename... ColumnTypes>
T& Crud<T>::insert(ColumnTypes&&... expression)
{
	static_cast<T*>(this)->cache.clear();

	std::string query;
	{ 
		std::stringstream ss;
		ss << "INSERT INTO " << static_cast<T*>(this)->name << " (";

		auto onColumn = [&ss, this](const auto&... iter) {
			(static_cast<void>( /// Make fold expression possible
				(ss << static_cast<T*>(this)->getColumnName(iter.l) << ", ") /// Column name
			), ...); /// Process fold expression
		};
		std::apply(onColumn, std::tuple(expression...));

		/// Remove last ", "
		query = ss.str();
		if (query.size() > 2)
			query.erase(query.end() - 2, query.end());
	}

	query += ") VALUES (";

	{
		std::stringstream ss;
		auto onValue = [&ss](const auto&... iter) {
			(static_cast<void>((ss << iter.r) << ", "), ...); /// Process fold expression
		};
		std::apply(onValue, std::tuple(expression...));

		/// Remove last ", "
		query += ss.str();
		if (query.size() > 2)
			query.erase(query.end() - 2, query.end());
	}

	query += ")";

	static_cast<T*>(this)->cache.emplace_back(std::move(query));

	return *(static_cast<T*>(this));
}

template<typename T>
template<typename... ColumnTypes>
T& Crud<T>::remove(ColumnTypes&&... cts)
{
	static_cast<T*>(this)->cache.clear();

	auto columnNames = static_cast<T*>(this)->getColumnNames(std::forward<ColumnTypes>(cts)...);

	std::stringstream ss;
	ss << "DELETE FROM " << static_cast<T*>(this)->name;

	static_cast<T*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<T*>(this));
}

template<typename T>
template<typename Expr>
T& Crud<T>::where(Expr expr)
{
	std::stringstream ss;
	ss << "WHERE " << this->processWhere(expr);

	static_cast<T*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<T*>(this));
}

template<typename T>
template<typename L, typename R>
std::string Crud<T>::processWhere(And<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename T>
template<typename L, typename R>
std::string Crud<T>::processWhere(Or<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename T>
template<typename Expr>
std::string Crud<T>::processWhere(const Expr& expr)
{
	std::stringstream ss;
	ss << static_cast<T*>(this)->getColumnName(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << expr.r;

	return ss.str();
}

} // namespace tsqb
} // namespace vist
