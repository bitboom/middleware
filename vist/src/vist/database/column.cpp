/*
 *  Copyright (c) 2015-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include "column.hpp"

namespace vist {
namespace database {

Column::Column(::sqlite3_stmt* stmt, int idx) : statement(stmt), index(idx)
{
}

std::string Column::getName() const
{
	return sqlite3_column_name(statement, index);
}

int Column::getInt() const
{
	return sqlite3_column_int(statement, index);
}

sqlite3_int64 Column::getInt64() const
{
	return sqlite3_column_int64(statement, index);
}

double Column::getDouble() const
{
	return sqlite3_column_double(statement, index);
}

const char* Column::getText() const
{
	return reinterpret_cast<const char*>(sqlite3_column_text(statement, index));
}

const void* Column::getBlob() const
{
	return sqlite3_column_blob(statement, index);
}

int Column::getType() const
{
	return sqlite3_column_type(statement, index);
}

int Column::getBytes() const
{
	return sqlite3_column_bytes(statement, index);
}

} // namespace database
} // namespace vist
