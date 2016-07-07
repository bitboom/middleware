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

#ifndef __DATABASE_CONNECTION_H__
#define __DATABASE_CONNECTION_H__

#include <string>

#include <sqlite3.h>

namespace database {

class Connection {
public:
	enum Mode {
		Create = SQLITE_OPEN_CREATE,
		ReadWrite = SQLITE_OPEN_READWRITE
	};

	Connection(const std::string& name, const int flags);
	~Connection();

	int exec(const std::string& query);
	bool isTableExists(const std::string& tableName);

	inline long long getLastInsertRowId() const noexcept
	{
		return sqlite3_last_insert_rowid(handle);
	}

	inline const std::string& getName() const noexcept
	{
		return filename;
	}

	inline int getErrorCode() const
	{
		return sqlite3_errcode(handle);
	}

	inline int getExtendedErrorCode() const
	{
		return sqlite3_extended_errcode(handle);
	}

	inline std::string getErrorMessage() const
	{
		return sqlite3_errmsg(handle);
	}

	inline sqlite3* get() const noexcept
	{
		return handle;
	}

private:
	sqlite3* handle;
	std::string filename;
};

} // namespace database
#endif //__DATABASE_CONNECTION_H__
