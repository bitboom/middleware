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
 * @file table.hxx
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Represent the table of SQL
 */

#pragma once

#include "crud.hxx"
#include "column.hxx"
#include "column-pack.hxx"
#include "tuple-helper.hxx"
#include "util.hxx"

#include <vector>
#include <string>
#include <sstream>

namespace tsqb {

template<typename... Columns>
class Table : public Crud<Table<Columns...>> {
public:
	virtual ~Table() = default;

	Table(const Table&) = delete;
	Table& operator=(const Table&) = delete;

	Table(Table&&) = default;
	Table& operator=(Table&&) = default;

	// Functions for Crud
	template<typename Cs>
	std::set<std::string> getTableNames(Cs&& tuple) const noexcept;
	template<typename Cs>
	std::vector<std::string> getColumnNames(Cs&& tuple) const noexcept;
	template<typename That>
	std::string getTableName(That&& type) const noexcept;
	template<typename ColumnType>
	std::string getColumnName(ColumnType&& type) const noexcept;

	std::vector<std::string> getColumnNames(void) const noexcept;

	template<typename That>
	bool compare(const That& that) const noexcept;

	int size() const noexcept;

	operator std::string();

	std::string name;
	std::vector<std::string> cache;

private:
	using ColumnPackType = internal::ColumnPack<Columns...>;
	using TableType = typename ColumnPackType::TableType;

	explicit Table(const std::string& name, ColumnPackType&& columnPack);

	template<typename ...Cs>
	friend Table<Cs...> make_table(const std::string& name, Cs&& ...columns);

	struct GetColumnNames {
		const ColumnPackType& columnPack;
		std::vector<std::string> names;

		GetColumnNames(const ColumnPackType& columnPack) : columnPack(columnPack) {}

		template <typename T>
		void operator()(T&& type)
		{
			auto name = this->columnPack.getName(std::forward<T>(type));
			if (!name.empty())
				names.emplace_back(name);
		}
	};

	ColumnPackType columnPack;
};

template<typename ...Columns>
Table<Columns...> make_table(const std::string& name, Columns&& ...cs)
{
	auto columnPack = internal::ColumnPack<Columns...>(std::forward<Columns>(cs)...);
	return Table<Columns...>(name, std::move(columnPack));
}

template<typename... Columns>
Table<Columns...>::Table(const std::string& name, ColumnPackType&& columnPack)
	: name(name), columnPack(std::move(columnPack)) {}

template<typename... Columns>
template<typename Cs>
std::set<std::string> Table<Columns...>::getTableNames(Cs&& tuple) const noexcept
{
	return {this->name};
}

template<typename... Columns>
template<typename Cs>
std::vector<std::string> Table<Columns...>::getColumnNames(Cs&& tuple) const noexcept
{
	GetColumnNames closure(this->columnPack);
	tuple_helper::for_each(std::forward<Cs>(tuple), closure);

	return closure.names;
}

template<typename... Columns>
template<typename That>
std::string Table<Columns...>::getTableName(That&& type) const noexcept
{
	return this->name;
}

template<typename... Columns>
template<typename ColumnType>
std::string Table<Columns...>::getColumnName(ColumnType&& type) const noexcept
{
	return this->columnPack.getName(std::forward<ColumnType>(type));
}

template<typename... Columns>
std::vector<std::string> Table<Columns...>::getColumnNames(void) const noexcept
{
	return this->columnPack.getNames();
}

template<typename... Columns>
template<typename That>
bool Table<Columns...>::compare(const That& that) const noexcept
{
	using This = TableType;
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
int Table<Columns...>::size() const noexcept
{
	return this->columnPack.size();
}

} // namespace tsqb
