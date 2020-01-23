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

namespace vist {
namespace tsqb {

template<typename T>
class Crud {
public:
	template<typename... ColumnTypes>
	T& select(ColumnTypes&&... cts);

	template<typename Type>
	T& select(Distinct<Type> distinct);

	template<typename TableType>
	T& selectAll(void);

	T& selectAll(void);

	template<typename... ColumnTypes>
	T& update(ColumnTypes&&... cts);

	template<typename... ColumnTypes>
	T& insert(ColumnTypes&&... cts);

	template<typename... ColumnTypes>
	T& remove(ColumnTypes&&... cts);

	template<typename Expr>
	T& where(Expr expr);

private:
	template<typename ColumnTuple>
	T& selectInternal(ColumnTuple&& ct, bool distinct = false);

	template<typename L, typename R>
	std::string processWhere(condition::And<L,R>& expr);

	template<typename L, typename R>
	std::string processWhere(condition::Or<L,R>& expr);

	template<typename Expr>
	std::string processWhere(Expr expr);
};

template<typename T>
template<typename... ColumnTypes>
T& Crud<T>::select(ColumnTypes&&... cts)
{
	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);

	return this->selectInternal(std::move(columnTuple));
}

template<typename T>
template<typename Type>
T& Crud<T>::select(Distinct<Type> distinct)
{
	return this->selectInternal(std::move(distinct.value), true);
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
template<typename ColumnTuple>
T& Crud<T>::selectInternal(ColumnTuple&& ct, bool distinct)
{
	static_cast<T*>(this)->cache.clear();

	auto columnNames = static_cast<T*>(this)->getColumnNames(std::move(ct));
	auto tableNames = static_cast<T*>(this)->getTableNames(std::move(ct));

	std::stringstream ss;
	ss << "SELECT ";

	if (distinct)
		ss << "DISTINCT ";

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
template<typename... ColumnTypes>
T& Crud<T>::update(ColumnTypes&&... cts)
{
	static_cast<T*>(this)->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = static_cast<T*>(this)->getColumnNames(std::move(columnTuple));

	std::stringstream ss;
	ss << "UPDATE " << static_cast<T*>(this)->name << " ";
	ss << "SET ";

	unsigned int i = 0;
	for (const auto& c : columnNames) {
		ss << c << " = ?";

		if (i++ < columnNames.size() - 1)
			ss << ", ";
	}

	static_cast<T*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<T*>(this));
}

template<typename T>
template<typename... ColumnTypes>
T& Crud<T>::insert(ColumnTypes&&... cts)
{
	static_cast<T*>(this)->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = static_cast<T*>(this)->getColumnNames(std::move(columnTuple));

	std::stringstream ss;
	ss << "INSERT INTO " << static_cast<T*>(this)->name << " (";

	const int columnCount = columnNames.size();
	for (int i = 0; i < columnCount; i++) {
		ss << columnNames[i];
		if (i < columnCount - 1)
			ss << ", ";
	}

	ss << ") VALUES (";

	for (int i = 0; i < columnCount; i++) {
		ss << "?";
		if (i < columnCount - 1)
			ss << ", ";
	}

	ss << ")";

	static_cast<T*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<T*>(this));
}

template<typename T>
template<typename... ColumnTypes>
T& Crud<T>::remove(ColumnTypes&&... cts)
{
	static_cast<T*>(this)->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = static_cast<T*>(this)->getColumnNames(std::move(columnTuple));

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
std::string Crud<T>::processWhere(condition::And<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename T>
template<typename L, typename R>
std::string Crud<T>::processWhere(condition::Or<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename T>
template<typename Expr>
std::string Crud<T>::processWhere(Expr expr)
{
	std::stringstream ss;
	ss << static_cast<T*>(this)->getColumnName(expr.l.type);
	ss << " " << std::string(expr) << " ?";

	return ss.str();
}

} // namespace tsqb
} // namespace vist
