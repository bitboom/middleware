/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "virtual-table.hpp"

#include <vist/client/query.hpp>
#include <vist/client/schema/policy.hpp>
#include <vist/client/schema/time.hpp>

#include <vist/exception.hpp>
#include <vist/stringfy.hpp>
#include <vist/logger.hpp>
#include <vist/query-builder.hpp>

#include <boost/lexical_cast.hpp>

namespace {

using namespace vist::tsqb;
using namespace vist::schema;

Table time { "time", Column("hour", &Time::hour),
					 Column("minutes", &Time::minutes),
					 Column("seconds", &Time::seconds) };

Table policyInt { "policy", Column("name", &Policy<int>::name),
							Column("value", &Policy<int>::value) };

Table policyStr { "policy", Column("name", &Policy<std::string>::name),
							Column("value", &Policy<std::string>::value) };

Database metaDB { "db", time, policyInt, policyStr };

} // anonymous namespace

namespace vist {

template <typename T>
VirtualRow<T>::VirtualRow()
{
	auto results = Query::Execute(metaDB.selectAll<T>());
	if (results.size() > 0)
		this->data = std::move(results[0]);
}

template <typename T>
VirtualRow<T>::VirtualRow(KeyValuePair&& kvp) : data(std::move(kvp))
{
}

template <typename T>
template<typename Struct, typename Member>
Member VirtualRow<T>::at(Member Struct::* field) const
{
	if (this->data.size() == 0)
		THROW(ErrCode::RuntimeError) << "Data is not exist.";

	/// TODO: Refactor
	Column anonymous("anonymous", field);
	std::string key = metaDB.getColumnName(anonymous);
	if (key.empty())
		THROW(ErrCode::RuntimeError) << "Column is not exist.";

	/// Convert "table.column" to "column"
	std::size_t pos = key.find(".");
	if (pos != std::string::npos && pos != key.size() - 1)
		key = key.substr(pos + 1);

	std::string value = this->data.at(key);
	if (value.empty()) {
		ERROR(VIST) << "The value of column[" << key << "] is empty.";
		return Member();
	}

	if (std::is_same<T, Policy<int>>::value && key == "value")
		return static_cast<Member>(Stringify::Restore(value));

	try {
		return boost::lexical_cast<Member>(value);
	} catch (...) {
		THROW(ErrCode::BadCast) << "Failed to casting [key]: " << key;
		return Member();
	}
}

template <typename T>
template <typename Struct, typename Member>
Member VirtualRow<T>::operator[](Member Struct::*field) const
{
	return this->at(field);
}

template <typename T>
VirtualTable<T>::VirtualTable()
{
	auto results = Query::Execute(metaDB.selectAll<T>());
	for (auto& row : results) {
		/// Filter unsafe(unmatched) type
		if (std::is_same<T, Policy<int>>::value && row["value"].find("I/") == std::string::npos)
			continue;
		else if (std::is_same<T, Policy<std::string>>::value &&
				 row["value"].find("S/") == std::string::npos)
			continue;

		this->rows.emplace_back(VirtualRow<T>(std::move(row)));
	}
}

/// Explicit instantiation
template class VirtualTable<Time>;
template class VirtualRow<Time>;
template int VirtualRow<Time>::at(int Time::*) const;
template int VirtualRow<Time>::operator[](int Time::*) const;

template class VirtualTable<Policy<int>>;
template class VirtualRow<Policy<int>>;
/// name column
template std::string VirtualRow<Policy<int>>::at(std::string Policy<int>::*) const;
template std::string VirtualRow<Policy<int>>::operator[](std::string Policy<int>::*) const;
/// value<T> column
template int VirtualRow<Policy<int>>::at(int Policy<int>::*) const;
template int VirtualRow<Policy<int>>::operator[](int Policy<int>::*) const;

template class VirtualTable<Policy<std::string>>;
template class VirtualRow<Policy<std::string>>;
template
std::string VirtualRow<Policy<std::string>>::at(std::string Policy<std::string>::*) const;
template
std::string VirtualRow<Policy<std::string>>::operator[](std::string Policy<std::string>::*) const;

} // namespace vist
