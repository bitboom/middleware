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

auto policy = make_table("policy",
						 make_column("name", &Policy::name),
						 make_column("value", &Policy::value));

auto metaDB = make_database("db", time, processes, policy);

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
		throw std::runtime_error("Data is not exist.");

	std::string key = metaDB.getColumnName(field);
	if (key.empty())
		throw std::runtime_error("Key is not exist.");

	/// Convert "table.column" to "column"
	std::size_t pos = key.find(".");
	if (pos != std::string::npos && pos != key.size() - 1)
		key = key.substr(pos + 1);

	std::string value = this->data.at(key);
	if (value.empty()) {
		ERROR(VIST, "The value of key[" << key << "] is not exist.");
		return Member();
	}

	try {
		return boost::lexical_cast<Member>(value);
	} catch (...) {
		ERROR(VIST, "Failed to casting [key]: " << key);
		return Member();
	}
}

template <typename T>
template<typename Struct, typename Member>
Member VirtualRow<T>::operator[](Member Struct::*field) const
{
	return this->at(field);
}

template <typename T>
VirtualTable<T>::VirtualTable()
{
	auto results = Query::Execute(metaDB.selectAll<T>());
	for (auto& r : results)
		this->dataset.emplace_back(VirtualRow<T>(std::move(r)));
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

template class VirtualTable<Policy>;
template class VirtualRow<Policy>;
template std::string VirtualRow<Policy>::at(std::string Policy::*) const;
template std::string VirtualRow<Policy>::operator[](std::string Policy::*) const;

} // namespace vist
