// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved
#include <osquery/core.h>
#include <osquery/registry_interface.h>
#include <osquery/sql.h>
#include <osquery/sql/sqlite_util.h>

namespace osquery {

const std::string kTestQuery{"SELECT * FROM test_table"};

// Starting with the dataset returned by createTestDB(), getTestDBResultStream
// returns a vector of std::pair's where pair.first is the query that would
// need to be performed on the dataset to make the results be pair.second
std::vector<std::pair<std::string, QueryDataTyped>> getTestDBResultStream();

// getTestDBExpectedResults returns the results of kTestQuery of the table that
// initially gets returned from createTestDB()
QueryDataTyped getTestDBExpectedResults();

// getSerializedRowColumnNames returns a vector of test column names that
// are in alphabetical order. If unordered_and_repeated is true, the
// vector includes a repeated column name and is in non-alphabetical order
ColumnNames getSerializedRowColumnNames(bool unordered_and_repeated);

} // namespace osquery
