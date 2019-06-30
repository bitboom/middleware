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
 * @file crud.hxx
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Represent four basic functions of CRUD
 */

#pragma once

#include "column.hxx"
#include "expression.hxx"

#include <string>
#include <sstream>

namespace tsqb {

template<typename Table>
class Crud {
public:
	using Self = Table;

	template<typename... ColumnTypes>
	Self& select(ColumnTypes&&... cts);

	template<typename Type>
	Self& select(Distinct<Type> distinct);

	Self& selectAll(void);

	template<typename... ColumnTypes>
	Self& update(ColumnTypes&&... cts);

	template<typename... ColumnTypes>
	Self& insert(ColumnTypes&&... cts);

	template<typename... ColumnTypes>
	Self& remove(ColumnTypes&&... cts);

	template<typename Expr>
	Self& where(Expr expr);

private:
	template<typename ColumnTuple>
	Self& selectInternal(ColumnTuple&& ct, bool distinct = false);

	template<typename L, typename R>
	std::string processWhere(condition::And<L,R>& expr);

	template<typename L, typename R>
	std::string processWhere(condition::Or<L,R>& expr);

	template<typename Expr>
	std::string processWhere(Expr expr);
};

template<typename Table>
template<typename... ColumnTypes>
Table& Crud<Table>::select(ColumnTypes&&... cts)
{
	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);

	return this->selectInternal(std::move(columnTuple));
}

template<typename Table>
template<typename Type>
Table& Crud<Table>::select(Distinct<Type> distinct)
{
	return this->selectInternal(std::move(distinct.value), true);
}

template<typename Table>
Table& Crud<Table>::selectAll(void)
{
	static_cast<Table*>(this)->cache.clear();

	std::stringstream ss;
	ss << "SELECT * FROM " << static_cast<Table*>(this)->name;

	static_cast<Table*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<Table*>(this));
}

template<typename Table>
template<typename ColumnTuple>
Table& Crud<Table>::selectInternal(ColumnTuple&& ct, bool distinct)
{
	static_cast<Table*>(this)->cache.clear();

	auto columnNames = static_cast<Table*>(this)->getColumnNames(std::move(ct));
	auto tableNames = static_cast<Table*>(this)->getTableNames(std::move(ct));

	std::stringstream ss;
	ss << "SELECT ";

	if (distinct)
		ss << "DISTINCT ";

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

	static_cast<Table*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<Table*>(this));
}

template<typename Table>
template<typename... ColumnTypes>
Table& Crud<Table>::update(ColumnTypes&&... cts)
{
	static_cast<Table*>(this)->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = static_cast<Table*>(this)->getColumnNames(std::move(columnTuple));

	std::stringstream ss;
	ss << "UPDATE " << static_cast<Table*>(this)->name << " ";
	ss << "SET ";

	int i = 0;
	for (const auto& c : columnNames) {
		ss << c << " = ?";

		if (i++ < columnNames.size() - 1)
			ss << ", ";
	}

	static_cast<Table*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<Table*>(this));
}

template<typename Table>
template<typename... ColumnTypes>
Table& Crud<Table>::insert(ColumnTypes&&... cts)
{
	static_cast<Table*>(this)->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = static_cast<Table*>(this)->getColumnNames(std::move(columnTuple));

	std::stringstream ss;
	ss << "INSERT INTO " << static_cast<Table*>(this)->name << " (";

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

	static_cast<Table*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<Table*>(this));
}

template<typename Table>
template<typename... ColumnTypes>
Table& Crud<Table>::remove(ColumnTypes&&... cts)
{
	static_cast<Table*>(this)->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = static_cast<Table*>(this)->getColumnNames(std::move(columnTuple));

	std::stringstream ss;
	ss << "DELETE FROM " << static_cast<Table*>(this)->name;

	static_cast<Table*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<Table*>(this));
}

template<typename Table>
template<typename Expr>
Table& Crud<Table>::where(Expr expr)
{
	std::stringstream ss;
	ss << "WHERE " << this->processWhere(expr);

	static_cast<Table*>(this)->cache.emplace_back(ss.str());

	return *(static_cast<Table*>(this));
}

template<typename Table>
template<typename L, typename R>
std::string Crud<Table>::processWhere(condition::And<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename Table>
template<typename L, typename R>
std::string Crud<Table>::processWhere(condition::Or<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename Table>
template<typename Expr>
std::string Crud<Table>::processWhere(Expr expr)
{
	std::stringstream ss;
	ss << static_cast<Table*>(this)->getColumnName(expr.l.type);
	ss << " " << std::string(expr) << " ?";

	return ss.str();
}

} // namespace tsqb
