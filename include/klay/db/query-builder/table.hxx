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

#include "column.hxx"
#include "table-impl.hxx"
#include "tuple-helper.hxx"
#include "expression.hxx"
#include "util.hxx"

#include <vector>
#include <string>
#include <sstream>

namespace qxx {

template<typename... Columns>
class Table {
public:
	using Self = Table<Columns...>;
	using ImplType = internal::TableImpl<Columns...>;
	using TableType = typename ImplType::TableType;

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

	template<typename That>
	bool compare(const That& that) const noexcept;

	operator std::string();

	template<typename ColumnType>
	std::string getColumnName(ColumnType&& type) const noexcept;

	std::string name;

private:
	explicit Table(const std::string& name, ImplType&& impl);

	template<typename ...Cs>
	friend Table<Cs...> make_table(const std::string& name, Cs&& ...columns);

	template<typename ColumnTuple>
	Self& selectInternal(ColumnTuple&& ct, bool distinct = false);

	template<typename Cs>
	std::vector<std::string> getColumnNames(Cs&& tuple);

	int size() const noexcept;

	std::vector<std::string> getColumnNames(void) const noexcept;

	struct GetColumnNames {
		ImplType impl;
		std::vector<std::string> names;

		GetColumnNames(const ImplType &impl) : impl(impl) {}

		template <typename T>
		void operator()(T&& type)
		{
			auto name = this->impl.getColumnName(std::forward<T>(type));
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

	ImplType impl;
	std::vector<std::string> cache;
};

template<typename ...Columns>
Table<Columns...> make_table(const std::string& name, Columns&& ...cs)
{
	auto impl = internal::TableImpl<Columns...>(std::forward<Columns>(cs)...);
	return Table<Columns...>(name, std::move(impl));
}

template<typename... Columns>
Table<Columns...>::Table(const std::string& name, ImplType&& impl)
	: name(name), impl(std::move(impl)) {}

template<typename... Columns>
template<typename... ColumnTypes>
Table<Columns...>& Table<Columns...>::select(ColumnTypes&&... cts)
{
	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);

	return this->selectInternal(std::move(columnTuple));
}

template<typename... Columns>
template<typename Type>
Table<Columns...>& Table<Columns...>::select(Distinct<Type> distinct)
{
	return this->selectInternal(std::move(distinct.value), true);
}

template<typename... Columns>
template<typename ColumnTuple>
Table<Columns...>& Table<Columns...>::selectInternal(ColumnTuple&& ct, bool distinct)
{
	this->cache.clear();

	auto columnNames = this->getColumnNames(std::move(ct));

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

	ss << " FROM " << this->name;

	cache.emplace_back(ss.str());

	return *this;
}

template<typename ...Columns>
Table<Columns...>& Table<Columns...>::selectAll(void)
{
	this->cache.clear();

	std::stringstream ss;
	ss << "SELECT * FROM " << this->name;

	cache.emplace_back(ss.str());

	return *this;
}

template<typename... Columns>
template<typename... ColumnTypes>
Table<Columns...>& Table<Columns...>::update(ColumnTypes&&... cts)
{
	this->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = this->getColumnNames(std::move(columnTuple));

	std::stringstream ss;
	ss << "UPDATE " << this->name << " ";
	ss << "SET ";

	int i = 0;
	for (const auto& c : columnNames) {
		ss << c << " = ?";

		if (i++ < columnNames.size() - 1)
			ss << ", ";
	}

	cache.emplace_back(ss.str());

	return *this;
}

template<typename... Columns>
template<typename... ColumnTypes>
Table<Columns...>& Table<Columns...>::insert(ColumnTypes&&... cts)
{
	this->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = this->getColumnNames(std::move(columnTuple));

	std::stringstream ss;
	ss << "INSERT INTO " << this->name << " (";

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

	cache.emplace_back(ss.str());

	return *this;
}

template<typename... Columns>
template<typename... ColumnTypes>
Table<Columns...>& Table<Columns...>::remove(ColumnTypes&&... cts)
{
	this->cache.clear();

	auto columnTuple = std::make_tuple(std::forward<ColumnTypes>(cts)...);
	auto columnNames = this->getColumnNames(std::move(columnTuple));

	std::stringstream ss;
	ss << "DELETE FROM " << this->name;

	cache.emplace_back(ss.str());

	return *this;
}

template<typename... Columns>
template<typename Expr>
Table<Columns...>& Table<Columns...>::where(Expr expr)
{
	std::stringstream ss;
	ss << "WHERE " << this->processWhere(expr);

	this->cache.emplace_back(ss.str());

	return *this;
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
template<typename That>
bool Table<Columns...>::compare(const That& that) const noexcept
{
	using This = TableType;
	return type::compare(This(), that);
}

template<typename... Columns>
template<typename Cs>
std::vector<std::string> Table<Columns...>::getColumnNames(Cs&& tuple)
{
	GetColumnNames closure(this->impl);
	tuple_helper::for_each(std::forward<Cs>(tuple), closure);

	return closure.names;
}

template<typename... Columns>
int Table<Columns...>::size() const noexcept
{
	return this->impl.size();
}

template<typename... Columns>
std::vector<std::string> Table<Columns...>::getColumnNames(void) const noexcept
{
	return this->impl.getColumnNames();
}

template<typename... Columns>
template<typename ColumnType>
std::string Table<Columns...>::getColumnName(ColumnType&& type) const noexcept
{
	return this->impl.getColumnName(std::forward<ColumnType>(type));
}

template<typename... Columns>
template<typename L, typename R>
std::string Table<Columns...>::processWhere(condition::And<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename... Columns>
template<typename L, typename R>
std::string Table<Columns...>::processWhere(condition::Or<L,R>& expr)
{
	std::stringstream ss;
	ss << this->processWhere(expr.l) << " ";
	ss << static_cast<std::string>(expr) << " ";
	ss << this->processWhere(expr.r);

	return ss.str();
}

template<typename... Columns>
template<typename Expr>
std::string Table<Columns...>::processWhere(Expr expr)
{
	std::stringstream ss;
	ss << this->impl.getColumnName(expr.l.type);
	ss << " " << std::string(expr) << " ?";

	return ss.str();
}

} // namespace qxx
