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

#include <tsqb.hxx>

#include <boost/lexical_cast.hpp>

namespace {

using namespace tsqb;
auto time = make_table("time",
					   make_column("hour", &Time::hour),
					   make_column("minutes", &Time::minutes),
					   make_column("seconds", &Time::seconds));

auto db = make_database("db", time);

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
template<typename Struct, typename Member>
Member Property<T>::get(Member Struct::* field)
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

	std::string value = this->data[key];
	if (value.empty())
		throw std::runtime_error("Value is not exist.");

	/// TODO(Sangwan): Catch boost::bad_lexical_cast
	return boost::lexical_cast<Member>(value);
}

/// Explicit instantiation
template class Property<Time>;
template int Property<Time>::get(int Time::*);

} // namespace osquery
