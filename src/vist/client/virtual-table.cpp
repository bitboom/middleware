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
#include <vist/client/schema/processes.hpp>
#include <vist/client/schema/time.hpp>

#include <vist/exception.hpp>
#include <vist/stringfy.hpp>
#include <vist/logger.hpp>
#include <vist/query-builder.hpp>

#include <boost/lexical_cast.hpp>

namespace {

using namespace vist::tsqb;
using namespace vist::schema;

auto time = make_table("time",
					   make_column("hour", &Time::hour),
					   make_column("minutes", &Time::minutes),
					   make_column("seconds", &Time::seconds));

auto processes = make_table("processes",
							make_column("pid", &Processes::pid),
							make_column("name", &Processes::name),
							make_column("path", &Processes::path),
							make_column("cmdline", &Processes::cmdline),
							make_column("uid", &Processes::uid),
							make_column("gid", &Processes::gid),
							make_column("euid", &Processes::euid),
							make_column("egid", &Processes::egid),
							make_column("on_disk", &Processes::on_disk),
							make_column("parent", &Processes::parent));

auto policyInt = make_table("policy",
						 make_column("name", &Policy<int>::name),
						 make_column("value", &Policy<int>::value));

auto policyStr = make_table("policy",
							make_column("name", &Policy<std::string>::name),
							make_column("value", &Policy<std::string>::value));

auto metaDB = make_database("db", time, processes, policyInt, policyStr);

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

	std::string key = metaDB.getColumnName(field);
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
		return static_cast<Member>(Stringfy::Restore(value));

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

		this->dataset.emplace_back(VirtualRow<T>(std::move(row)));
	}
}

/// Explicit instantiation
template class VirtualTable<Time>;
template class VirtualRow<Time>;
template int VirtualRow<Time>::at(int Time::*) const;
template int VirtualRow<Time>::operator[](int Time::*) const;

template class VirtualTable<Processes>;
template class VirtualRow<Processes>;
template int VirtualRow<Processes>::at(int Processes::*) const;
template int VirtualRow<Processes>::operator[](int Processes::*) const;
template long long int VirtualRow<Processes>::at(long long int Processes::*) const;
template long long int VirtualRow<Processes>::operator[](long long int Processes::*) const;
template std::string VirtualRow<Processes>::at(std::string Processes::*) const;
template std::string VirtualRow<Processes>::operator[](std::string Processes::*) const;

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
