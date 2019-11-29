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

#include <vist/exception.hpp>

#include <vist/database/column.hpp>
#include <vist/database/statement.hpp>
#include <vist/database/connection.hpp>

#include <string>

#include <gtest/gtest.h>

namespace {

const std::string TEST_DB_PATH = "/tmp/vist-test.db";

} // anonymous namespace

using namespace vist;

TEST(DatabaseTests, database)
{
	std::string query = "CREATE TABLE IF NOT EXISTS CLIENT("     \
						"ID INTEGER PRIMARY KEY AUTOINCREMENT,"  \
						"PKG TEXT,"                              \
						"KEY TEXT,"                              \
						"IS_USED INTEGER,"                       \
						"USER INTEGER)";

	try {
		database::Connection db(TEST_DB_PATH);
		db.exec(query);
	} catch (const vist::Exception<ErrCode>& e) {
		EXPECT_TRUE(false) << e.what();
	}
}

TEST(DatabaseTests, invalid_statement)
{
	bool raised = false;

	try {
		database::Connection db(TEST_DB_PATH);
		database::Statement stmt(db, "INVALID STATEMENT");
	} catch (const vist::Exception<ErrCode>&) {
		raised = true;
	}

	EXPECT_TRUE(raised);
}

TEST(DatabaseTests, column_bind_index1)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, ?, ?, ?, ?)";

	bool raised = false;
	try {
		const char *str = "PACKAGE";
		void *blob = (void *)str;
		double user = 5001;
		sqlite3_int64 used = 1;
		std::string key = "test key";

		database::Connection db(TEST_DB_PATH);
		database::Statement stmt(db, query);
		stmt.bind(1, blob, 8);
		stmt.bind(2, key);
		stmt.bind(3, used);
		stmt.bind(4, user);
		stmt.exec();
		database::Statement select(db, "SELECT * FROM CLIENT");

		EXPECT_EQ(5, select.getColumnCount());
		stmt.clearBindings();
		stmt.reset();
	} catch (const vist::Exception<ErrCode>&) {
		raised = true;
	}

	EXPECT_FALSE(raised);
}

TEST(DatabaseTests, column_bind_index2)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, ?, ?, ?, ?)";

	try {
		database::Connection db(TEST_DB_PATH);
		database::Statement stmt(db, query);
		stmt.bind(1, "TEST PACKAGE");
		stmt.bind(2, "TEST KEY");
		stmt.bind(3, false);
		stmt.bind(4, 5001);
		stmt.exec();
	} catch (const vist::Exception<ErrCode>& e) {
		EXPECT_TRUE(false) << e.what();
	}
}

TEST(DatabaseTests, column_bind_null)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, :PKG, :KEY, :IS_USED, :USER)";

	try {
		database::Connection db(TEST_DB_PATH);
		database::Statement stmt(db, query);
		stmt.bind(":PKG");
		stmt.bind(2);
	} catch (const vist::Exception<ErrCode>& e) {
		EXPECT_TRUE(false) << e.what();
	}
}

TEST(DatabaseTests, column_bind_name1)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, :PKG, :KEY, :IS_USED, :USER)";

	try {
		database::Connection db(TEST_DB_PATH);
		database::Statement stmt(db, query);
		stmt.bind(":PKG", "TEST PACKAGE");
		stmt.bind(":KEY", "TEST KEY");
		stmt.bind(":IS_USED", true);
		stmt.bind(":USER", 5001);
		stmt.exec();
	} catch (const vist::Exception<ErrCode>& e) {
		EXPECT_TRUE(false) << e.what();
	}
}

TEST(DatabaseTests, column_bind_name2)
{
	std::string query = "INSERT INTO CLIENT VALUES (NULL, :PKG, :KEY, :IS_USED, :USER)";

	try {
		const char *str = "PACKAGE";
		void *blob = (void *)str;
		double user = 5001;
		sqlite3_int64 used = 1;
		std::string key = "test key";

		database::Connection db(TEST_DB_PATH);
		database::Statement stmt(db, query);
		stmt.bind(":PKG", blob, 8);
		stmt.bind(":KEY", key);
		stmt.bind(":IS_USED", used);
		stmt.bind(":USER", user);
		stmt.exec();
	} catch (const vist::Exception<ErrCode>& e) {
		EXPECT_TRUE(false) << e.what();
	}
}

TEST(DatabaseTests, column)
{
	try {
		database::Connection db(TEST_DB_PATH);
		database::Statement select(db, "SELECT * FROM CLIENT");
		while (select.step()) {
			for (int  i = 0; i < select.getColumnCount(); i++) {
				if (select.isNullColumn(i)) {
					continue;
				}
				select.getColumnName(i);
			}

			database::Column id = select.getColumn(0);
			database::Column pkg = select.getColumn(1);
			database::Column key = select.getColumn(2);
			database::Column used = select.getColumn(3);

			id.getInt();
			pkg.getText();
			key.getText();
			used.getInt();
		}
	} catch (const vist::Exception<ErrCode>& e) {
		EXPECT_TRUE(false) << e.what();
	}
}
