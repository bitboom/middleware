/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <osquery/core.h>
#include <osquery/registry_interface.h>
#include <osquery/sql.h>
#include <osquery/sql/sqlite_util.h>
#include <osquery/sql/tests/sql_test_utils.h>

#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>

namespace osquery {
class SQLiteUtilTests : public testing::Test {
public:
	void SetUp() override
	{
		registryAndPluginInit();
	}
};

std::shared_ptr<SQLiteDBInstance> getTestDBC()
{
	auto dbc = SQLiteDBManager::getUnique();
	char* err = nullptr;
	std::vector<std::string> queries = {
		"CREATE TABLE test_table (username varchar(30) primary key, age int)",
		"INSERT INTO test_table VALUES (\"mike\", 23)",
		"INSERT INTO test_table VALUES (\"matt\", 24)"
	};

	for (auto q : queries) {
		sqlite3_exec(dbc->db(), q.c_str(), nullptr, nullptr, &err);
		if (err != nullptr) {
			throw std::domain_error(std::string("Cannot create testing DBC's db: ") +
									err);
		}
	}

	return dbc;
}

TEST_F(SQLiteUtilTests, test_zero_as_float_doesnt_convert_to_int)
{
	auto sql = SQL("SELECT 0.0 as zero");
	EXPECT_TRUE(sql.ok());
	EXPECT_EQ(sql.rows().size(), 1U);
	Row r;
	r["zero"] = "0.0";
	EXPECT_EQ(sql.rows()[0], r);
}

TEST_F(SQLiteUtilTests, test_precision_is_maintained)
{
	auto sql = SQL("SELECT 0.123456789 as high_precision, 0.12 as low_precision");
	EXPECT_TRUE(sql.ok());
	EXPECT_EQ(sql.rows().size(), 1U);
	Row r;
	r["high_precision"] = "0.123456789";
	r["low_precision"] = "0.12";
	EXPECT_EQ(sql.rows()[0], r);
}

TEST_F(SQLiteUtilTests, test_sqlite_instance_manager)
{
	auto dbc1 = SQLiteDBManager::get();
	auto dbc2 = SQLiteDBManager::get();
	EXPECT_NE(dbc1->db(), dbc2->db());
	EXPECT_EQ(dbc1->db(), dbc1->db());
}

TEST_F(SQLiteUtilTests, test_sqlite_instance)
{
	// Don't do this at home kids.
	// Keep a copy of the internal DB and let the SQLiteDBInstance go oos.
	auto internal_db = SQLiteDBManager::get()->db();
	// Compare the internal DB to another request with no SQLiteDBInstances
	// in scope, meaning the primary will be returned.
	EXPECT_EQ(internal_db, SQLiteDBManager::get()->db());
}

TEST_F(SQLiteUtilTests, test_reset)
{
	auto internal_db = SQLiteDBManager::get()->db();
	ASSERT_NE(nullptr, internal_db);

	sqlite3_exec(internal_db,
				 "create view test_view as select 'test';",
				 nullptr,
				 nullptr,
				 nullptr);

	SQLiteDBManager::resetPrimary();
	auto instance = SQLiteDBManager::get();

	QueryDataTyped results;
	queryInternal("select * from test_view", results, instance);

	// Assume the internal (primary) database we reset and recreated.
	EXPECT_EQ(results.size(), 0U);
}

TEST_F(SQLiteUtilTests, test_direct_query_execution)
{
	auto dbc = getTestDBC();
	QueryDataTyped results;
	auto status = queryInternal(kTestQuery, results, dbc);
	EXPECT_TRUE(status.ok());
	EXPECT_EQ(results, getTestDBExpectedResults());
}

TEST_F(SQLiteUtilTests, test_aggregate_query)
{
	auto dbc = getTestDBC();
	QueryDataTyped results;
	auto status = queryInternal(kTestQuery, results, dbc);
	EXPECT_TRUE(status.ok());
	EXPECT_EQ(results, getTestDBExpectedResults());
}

TEST_F(SQLiteUtilTests, test_no_results_query)
{
	auto dbc = getTestDBC();
	QueryDataTyped results;
	auto status = queryInternal(
					  "select * from test_table where username=\"A_NON_EXISTENT_NAME\"",
					  results,
					  dbc);
	EXPECT_TRUE(status.ok());
}

TEST_F(SQLiteUtilTests, test_whitespace_query)
{
	auto dbc = getTestDBC();
	QueryDataTyped results;
	auto status = queryInternal("     ", results, dbc);
	EXPECT_TRUE(status.ok());
}

TEST_F(SQLiteUtilTests, test_get_test_db_result_stream)
{
	auto dbc = getTestDBC();
	auto results = getTestDBResultStream();
	for (auto r : results) {
		char* err_char = nullptr;
		sqlite3_exec(dbc->db(), (r.first).c_str(), nullptr, nullptr, &err_char);
		EXPECT_TRUE(err_char == nullptr);
		if (err_char != nullptr) {
			sqlite3_free(err_char);
			ASSERT_TRUE(false);
		}

		QueryDataTyped expected;
		auto status = queryInternal(kTestQuery, expected, dbc);
		EXPECT_EQ(expected, r.second);
	}
}

} // namespace osquery
