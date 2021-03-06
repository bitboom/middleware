/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <string>
#include <iostream>

#include <klay/exception.h>
#include <klay/db/column.h>
#include <klay/db/statement.h>
#include <klay/db/connection.h>

namespace klay {
namespace database {

Statement::Statement(const Connection& db, const std::string& query) :
	statement(nullptr),
	columnCount(0),
	validRow(false)
{
	if (SQLITE_OK != ::sqlite3_prepare_v2(db.get(),
										  query.c_str(),
										  query.size(),
										  &statement,
										  NULL)) {
		throw klay::Exception(db.getErrorMessage());
	}

	columnCount = sqlite3_column_count(statement);
}

Statement::~Statement()
{
	::sqlite3_finalize(statement);
}

void Statement::reset()
{
	if (::sqlite3_reset(statement) != SQLITE_OK) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::clearBindings()
{
	if (::sqlite3_clear_bindings(statement) != SQLITE_OK) {
		throw klay::Exception(getErrorMessage());
	}
}

std::string Statement::getErrorMessage() const
{
	return ::sqlite3_errmsg(::sqlite3_db_handle(statement));
}

std::string Statement::getErrorMessage(int errorCode) const
{
	return ::sqlite3_errstr(errorCode);
}

bool Statement::step()
{
	if (SQLITE_ROW == ::sqlite3_step(statement)) {
		return (validRow = true);
	}

	return (validRow = false);
}

int Statement::exec()
{
	if (SQLITE_DONE == ::sqlite3_step(statement)) {
		validRow = false;
	}

	return sqlite3_changes(sqlite3_db_handle(statement));
}

Column Statement::getColumn(const int index)
{
	if (!validRow || (index >= columnCount)) {
		throw klay::Exception(getErrorMessage(SQLITE_RANGE));
	}

	return Column(*this, index);
}

bool Statement::isNullColumn(const int index) const
{
	if (!validRow || (index >= columnCount)) {
		throw klay::Exception(getErrorMessage(SQLITE_RANGE));
	}

	return (SQLITE_NULL == sqlite3_column_type(statement, index));
}

std::string Statement::getColumnName(const int index) const
{
	if (index >= columnCount) {
		throw klay::Exception(getErrorMessage(SQLITE_RANGE));
	}

	return sqlite3_column_name(statement, index);
}


void Statement::bind(const int index, const int& value)
{
	if (SQLITE_OK != ::sqlite3_bind_int(statement, index, value)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const int index, const sqlite3_int64& value)
{
	if (SQLITE_OK != ::sqlite3_bind_int64(statement, index, value)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const int index, const double& value)
{
	if (SQLITE_OK != ::sqlite3_bind_double(statement, index, value)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const int index, const char* value)
{
	if (SQLITE_OK != ::sqlite3_bind_text(statement, index, value, -1, SQLITE_TRANSIENT)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const int index, const std::string& value)
{
	if (SQLITE_OK != ::sqlite3_bind_text(statement, index, value.c_str(),
										 static_cast<int>(value.size()), SQLITE_TRANSIENT)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const int index, const void* value, const int size)
{
	if (SQLITE_OK != ::sqlite3_bind_blob(statement, index, value, size, SQLITE_TRANSIENT)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const int index)
{
	if (SQLITE_OK != ::sqlite3_bind_null(statement, index)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const std::string& name, const int& value)
{
	int index = sqlite3_bind_parameter_index(statement, name.c_str());
	if (SQLITE_OK != sqlite3_bind_int(statement, index, value)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const std::string& name, const sqlite3_int64& value)
{
	int index = sqlite3_bind_parameter_index(statement, name.c_str());
	if (SQLITE_OK != ::sqlite3_bind_int64(statement, index, value)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const std::string& name, const double& value)
{
	int index = sqlite3_bind_parameter_index(statement, name.c_str());
	if (SQLITE_OK != ::sqlite3_bind_double(statement, index, value)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const std::string& name, const std::string& value)
{
	int index = sqlite3_bind_parameter_index(statement, name.c_str());
	if (SQLITE_OK != ::sqlite3_bind_text(statement, index, value.c_str(),
										 static_cast<int>(value.size()), SQLITE_TRANSIENT)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const std::string& name, const char* value)
{
	int index = sqlite3_bind_parameter_index(statement, name.c_str());
	if (SQLITE_OK != ::sqlite3_bind_text(statement, index, value, -1, SQLITE_TRANSIENT)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const std::string& name, const void* value, const int size)
{
	int index = sqlite3_bind_parameter_index(statement, name.c_str());
	if (SQLITE_OK != ::sqlite3_bind_blob(statement, index, value, size, SQLITE_TRANSIENT)) {
		throw klay::Exception(getErrorMessage());
	}
}

void Statement::bind(const std::string& name)
{
	int index = sqlite3_bind_parameter_index(statement, name.c_str());
	if (SQLITE_OK != ::sqlite3_bind_null(statement, index)) {
		throw klay::Exception(getErrorMessage());
	}
}

} // namespace database
} // namespace klay
