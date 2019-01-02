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
#include <cstring>

#include <klay/exception.h>
#include <klay/db/connection.h>

namespace klay {
namespace database {

Connection::Connection(const std::string& name, const int flags, bool integrityCheck) :
	handle(nullptr), filename(name)
{
	if (::sqlite3_open_v2(filename.c_str(), &handle, flags, NULL)) {
		throw klay::Exception(getErrorMessage());
	}

	if (integrityCheck) {
		bool verified = false;
		sqlite3_stmt *integrity = NULL;
		if (::sqlite3_prepare_v2(handle, "PRAGMA integrity_check;", -1, &integrity, NULL) == SQLITE_OK ) {
			while (::sqlite3_step(integrity) == SQLITE_ROW) {
				const unsigned char *result = ::sqlite3_column_text(integrity, 0);
				if (result && ::strcmp((const char *)result, (const char *)"ok") == 0) {
					verified = true;
					break;
				}
			}

			::sqlite3_finalize(integrity);
		}

		if (!verified) {
			::sqlite3_close(handle);
			throw klay::Exception("Malformed database");
		}
	}
}

Connection::~Connection()
{
	::sqlite3_close(handle);
}

int Connection::exec(const std::string& query)
{
	if (::sqlite3_exec(handle, query.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
		throw klay::Exception(getErrorMessage());
	}

	return ::sqlite3_changes(handle);
}

} // namespace database
} // namespace klay
