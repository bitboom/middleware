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
/*
 * @file property.cpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Implementation of Property
 */

#include <property.h>

#include <schema/time.h>
#include <schema/processes.h>

#include "../service/vist.h"

#include <osquery/logger.h>

#include <tsqb.hxx>

#include <boost/lexical_cast.hpp>

namespace {

using namespace tsqb;
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

auto db = make_database("db", time, processes);

} // anonymous namespace

namespace vist {

template <typename T>
Property<T>::Property()
{
	auto results = Vist::Query(db.selectAll<T>());
	if (results.size() > 0)
		this->data = std::move(results[0]);
}

template <typename T>
Property<T>::Property(KeyValuePair&& kvp) : data(std::move(kvp))
{
}

template <typename T>
template<typename Struct, typename Member>
Member Property<T>::at(Member Struct::* field) const
{
	if (this->data.size() == 0)
		throw std::runtime_error("Data is not exist.");

	std::string key = db.getColumnName(field);
	if (key.empty())
		throw std::runtime_error("Key is not exist.");

	/// Convert "table.column" to "column"
	std::size_t pos = key.find(".");
	if (pos != std::string::npos && pos != key.size() - 1)
		key = key.substr(pos + 1);

	std::string value = this->data.at(key);
	if (value.empty()) {
		LOG(ERROR) << "The value of key[" << key << "] is not exist.";
		return Member();
	}

	try {
		return boost::lexical_cast<Member>(value);
	} catch (...) {
		LOG(ERROR) << "Failed to casting [key]: " << key;
		return Member();
	}
}

template <typename T>
template<typename Struct, typename Member>
Member Property<T>::operator[](Member Struct::*field) const
{
	return this->at(field);
}

template <typename T>
Properties<T>::Properties()
{
	auto results = Vist::Query(db.selectAll<T>());
	for (auto& r : results)
		this->datas.emplace_back(Property<T>(std::move(r)));
}

/// Explicit instantiation
template class Property<Time>;
template class Properties<Time>;
template int Property<Time>::at(int Time::*) const;
template int Property<Time>::operator[](int Time::*) const;

template class Property<Processes>;
template class Properties<Processes>;
template int Property<Processes>::at(int Processes::*) const;
template int Property<Processes>::operator[](int Processes::*) const;
template long long int Property<Processes>::at(long long int Processes::*) const;
template long long int Property<Processes>::operator[](long long int Processes::*) const;
template std::string Property<Processes>::at(std::string Processes::*) const;
template std::string Property<Processes>::operator[](std::string Processes::*) const;

} // namespace vist
