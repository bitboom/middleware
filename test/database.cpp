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

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <iostream>
#include <stdexcept>

#include <klay/exception.h>
#include <klay/db/column.h>
#include <klay/db/statement.h>
#include <klay/db/connection.h>

#include <klay/testbench.h>

const std::string TestbenchDataSource = "/tmp/dpm-testbench.db";

TESTCASE(DatabaseTest)
{
	std::string query = "CREATE TABLE IF NOT EXISTS CLIENT("     \
						"ID INTEGER PRIMARY KEY AUTOINCREMENT,"  \
						"PKG TEXT,"                              \
						"KEY TEXT,"                              \
						"IS_USED INTEGER,"                       \
						"USER INTEGER)";

	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		db.exec(query);
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(InvalidStatementTest)
{
	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement stmt(db, "INVALID STATEMENT");
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(InvalidStatementTest2)
{
	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		db.exec("INVALID");
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(ColumnBindTestWithIndex1)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, ?, ?, ?, ?)";

	try {
		const char *str = "PACKAGE";
		void *blob = (void *)str;
		double user = 5001;
		sqlite3_int64 used = 1;
		std::string key = "test key";

		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement stmt(db, query);
		stmt.bind(1, blob, 8);
		stmt.bind(2, key);
		stmt.bind(3, used);
		stmt.bind(4, user);
		stmt.exec();
		database::Statement select(db, "SELECT * FROM CLIENT");

		TEST_EXPECT(5, select.getColumnCount());
		stmt.clearBindings();
		stmt.reset();
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(ColumnBindTestWithIndex2)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, ?, ?, ?, ?)";

	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement stmt(db, query);
		stmt.bind(1, "TEST PACKAGE");
		stmt.bind(2, "TEST KEY");
		stmt.bind(3, false);
		stmt.bind(4, 5001);
		stmt.exec();
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(ColumnBindTestWithName1)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, :PKG, :KEY, :IS_USED, :USER)";

	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement stmt(db, query);
		stmt.bind(":PKG", "TEST PACKAGE");
		stmt.bind(":KEY", "TEST KEY");
		stmt.bind(":IS_USED", true);
		stmt.bind(":USER", 5001);
		stmt.exec();
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(ColumnBindNullTest)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, :PKG, :KEY, :IS_USED, :USER)";

	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement stmt(db, query);
		stmt.bind(":PKG");
		stmt.bind(2);
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(ColumnBindTestWithName2)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, :PKG, :KEY, :IS_USED, :USER)";

	try {
		const char *str = "PACKAGE";
		void *blob = (void *)str;
		double user = 5001;
		sqlite3_int64 used = 1;
		std::string key = "test key";

		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement stmt(db, query);
		stmt.bind(":PKG", blob, 8);
		stmt.bind(":KEY", key);
		stmt.bind(":IS_USED", used);
		stmt.bind(":USER", user);
		stmt.exec();
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(ColumnTest)
{
	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement select(db, "SELECT * FROM CLIENT");
		while (select.step()) {
			for (int  i = 0; i < select.getColumnCount(); i++) {
				if (select.isNullColumn(i)) {
					continue;
				}
				select.getColumnName(i);
			}
			std::cout << std::endl;

			database::Column id = select.getColumn(0);
			database::Column pkg = select.getColumn(1);
			database::Column key = select.getColumn(2);
			database::Column used = select.getColumn(3);

			id.getName(); id.getInt(); id.getInt64(); id.getDouble(); id.getType(); id.getBytes(); id.getDouble();
			pkg.getName(); pkg.getText(); pkg.getType(); pkg.getBytes(); pkg.getBlob();
			key.getName(); key.getText(); key.getType(); key.getBytes(); key.getBlob();
			used.getName(); used.getInt(); used.getInt64(); used.getDouble(); used.getType(); used.getBytes();
		}
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(ColumnOutRange1)
{
	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement select(db, "SELECT * FROM CLIENT");
		select.step();
		database::Column column = select.getColumn(32);
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(ColumnOutRange2)
{
	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement select(db, "SELECT * FROM CLIENT");
		select.step();
		select.isNullColumn(32);
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(ColumnOutRange3)
{
	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement select(db, "SELECT * FROM CLIENT");
		select.step();
		select.getColumnName(32);
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(ColumnBindOutRange1)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, :PKG, :KEY, :IS_USED, :USER)";

	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement stmt(db, query);
		try {
			stmt.bind(":TPK", "TEST PACKAGE");
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(":TPK", (int)10);
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(":TPK", (sqlite3_int64)10);
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(":TPK", (double)10);
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(":TPK", "invalid");
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(":TPK", std::string("invalid"));
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(":TPK", (void *)NULL, 12);
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(":TPK");
		} catch (runtime::Exception& e) {
		}

		stmt.exec();
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(ColumnBindOutRange2)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, :PKG, :KEY, :IS_USED, :USER)";

	try {
		database::Connection db(TestbenchDataSource, database::Connection::ReadWrite | database::Connection::Create);
		database::Statement stmt(db, query);
		try {
			stmt.bind(32, "TEST PACKAGE");
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(32, (int)10);
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(32, (sqlite3_int64)10);
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(32, (double)10);
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(32, "invalid");
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(32, std::string("invalid"));
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(32, (void *)NULL, 12);
		} catch (runtime::Exception& e) {
		}

		try {
			stmt.bind(32);
		} catch (runtime::Exception& e) {
		}

		stmt.exec();
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(InvalidDB)
{
	try {
		database::Connection db("/tmp/invalid.db", database::Connection::ReadWrite);
	} catch (runtime::Exception& e) {
	}
}
