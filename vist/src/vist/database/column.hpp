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
/*
 * @file   connection.hpp
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @brief  Define Column of sqlite3 database.
 */

#pragma once

#include <sqlite3.h>

#include <string>

namespace vist {
namespace database {

class Column final {
public:
	Column(::sqlite3_stmt* stmt, int idx);
	~Column() = default;

	std::string getName() const;
	sqlite3_int64 getInt64() const;
	const char* getText() const;
	double getDouble() const;
	const void* getBlob() const;
	int getInt() const;
	int getType() const;
	int getBytes() const;

	int size() const
	{
		return getBytes();
	}

	operator int() const
	{
		return getInt();
	}

	operator sqlite3_int64() const
	{
		return getInt64();
	}

	operator double() const
	{
		return getDouble();
	}

	operator const char* () const
	{
		return getText();
	}

	operator const void* () const
	{
		return getBlob();
	}

private:
	::sqlite3_stmt* statement;
	int index;
};

} // namespace database
} // namespace vist
