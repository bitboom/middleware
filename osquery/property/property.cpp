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

#include <osquery_manager.h>
#include <property.h>

#include <schema/time.h>
#include <schema/processes.h>
#include <schema/users.h>
#include <schema/groups.h>
#include <schema/memory-map.h>

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
//							make_column("wired_size", &Processes::wired_size),
							make_column("resident_size", &Processes::resident_size),
							make_column("phys_footprint", &Processes::phys_footprint),
							make_column("user_time", &Processes::user_time),
							make_column("system_time", &Processes::system_time),
							make_column("start_time", &Processes::start_time),
							make_column("parent", &Processes::parent));

auto users = make_table("users",
						make_column("uid", &Users::uid),
						make_column("gid", &Users::gid),
						make_column("uid_signed", &Users::uid_signed),
						make_column("gid_signed", &Users::gid_signed),
						make_column("username", &Users::username),
						make_column("description", &Users::description),
						make_column("directory", &Users::directory),
						make_column("shell", &Users::shell));

auto groups = make_table("groups",
						 make_column("gid", &Groups::gid),
						 make_column("gid_signed", &Groups::gid_signed),
						 make_column("groupname", &Groups::groupname));

auto memoryMap = make_table("memory_map",
						 make_column("region", &MemoryMap::region),
						 make_column("type", &MemoryMap::type),
						 make_column("start", &MemoryMap::start),
						 make_column("end", &MemoryMap::end));

auto db = make_database("db", time, processes, users, groups, memoryMap);

} // anonymous namespace

namespace osquery {

template <typename T>
Property<T>::Property()
{
	auto results = OsqueryManager::execute(db.selectAll<T>());
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
		LOG(ERROR) << "Key: " << key << "is not exist.";
		return Member();
	} else {
		/// TODO(Sangwan): Catch boost::bad_lexical_cast
		return boost::lexical_cast<Member>(value);
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
	auto results = OsqueryManager::execute(db.selectAll<T>());
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

template class Property<Users>;
template class Properties<Users>;
template long long int Property<Users>::at(long long int Users::*) const;
template long long int Property<Users>::operator[](long long int Users::*) const;
template unsigned long long int Property<Users>::at(unsigned long long int Users::*) const;
template unsigned long long int Property<Users>::operator[](unsigned long long int Users::*) const;
template std::string Property<Users>::at(std::string Users::*) const;
template std::string Property<Users>::operator[](std::string Users::*) const;

template class Property<Groups>;
template class Properties<Groups>;
template long long int Property<Groups>::at(long long int Groups::*) const;
template long long int Property<Groups>::operator[](long long int Groups::*) const;
template unsigned long long int Property<Groups>::at(unsigned long long int Groups::*) const;
template unsigned long long int Property<Groups>::operator[](unsigned long long int Groups::*) const;
template std::string Property<Groups>::at(std::string Groups::*) const;
template std::string Property<Groups>::operator[](std::string Groups::*) const;

template class Property<MemoryMap>;
template class Properties<MemoryMap>;
template int Property<MemoryMap>::at(int MemoryMap::*) const;
template int Property<MemoryMap>::operator[](int MemoryMap::*) const;
template std::string Property<MemoryMap>::at(std::string MemoryMap::*) const;
template std::string Property<MemoryMap>::operator[](std::string MemoryMap::*) const;

} // namespace osquery
