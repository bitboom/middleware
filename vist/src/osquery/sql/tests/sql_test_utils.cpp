// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved
#include <osquery/sql/tests/sql_test_utils.h>

namespace osquery {

QueryDataTyped getTestDBExpectedResults()
{
	QueryDataTyped d;
	RowTyped row1;
	row1["username"] = "mike";
	row1["age"] = 23LL;
	d.push_back(row1);
	RowTyped row2;
	row2["username"] = "matt";
	row2["age"] = 24LL;
	d.push_back(row2);
	return d;
}

std::vector<std::pair<std::string, QueryDataTyped>> getTestDBResultStream()
{
	std::vector<std::pair<std::string, QueryDataTyped>> results;

	std::string q2 =
		R"(INSERT INTO test_table (username, age) VALUES ("joe", 25))";
	QueryDataTyped d2;
	RowTyped row2_1;
	row2_1["username"] = "mike";
	row2_1["age"] = 23LL;
	d2.push_back(row2_1);
	RowTyped row2_2;
	row2_2["username"] = "matt";
	row2_2["age"] = 24LL;
	d2.push_back(row2_2);
	RowTyped row2_3;
	row2_3["username"] = "joe";
	row2_3["age"] = 25LL;
	d2.push_back(row2_3);
	results.push_back(std::make_pair(q2, d2));

	std::string q3 = R"(UPDATE test_table SET age = 27 WHERE username = "matt")";
	QueryDataTyped d3;
	RowTyped row3_1;
	row3_1["username"] = "mike";
	row3_1["age"] = 23LL;
	d3.push_back(row3_1);
	RowTyped row3_2;
	row3_2["username"] = "matt";
	row3_2["age"] = 27LL;
	d3.push_back(row3_2);
	RowTyped row3_3;
	row3_3["username"] = "joe";
	row3_3["age"] = 25LL;
	d3.push_back(row3_3);
	results.push_back(std::make_pair(q3, d3));

	std::string q4 =
		R"(DELETE FROM test_table WHERE username = "matt" AND age = 27)";
	QueryDataTyped d4;
	RowTyped row4_1;
	row4_1["username"] = "mike";
	row4_1["age"] = 23LL;
	d4.push_back(row4_1);
	RowTyped row4_2;
	row4_2["username"] = "joe";
	row4_2["age"] = 25LL;
	d4.push_back(row4_2);
	results.push_back(std::make_pair(q4, d4));

	return results;
}

ColumnNames getSerializedRowColumnNames(bool unordered_and_repeated)
{
	ColumnNames cn;
	if (unordered_and_repeated) {
		cn.push_back("repeated_column");
	}
	cn.push_back("alphabetical");
	cn.push_back("foo");
	cn.push_back("meaning_of_life");
	cn.push_back("repeated_column");
	return cn;
}

} // namespace osquery
