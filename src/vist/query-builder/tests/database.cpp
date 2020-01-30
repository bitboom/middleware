/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <gtest/gtest.h>

#include <vist/query-builder/column.hpp>
#include <vist/query-builder/database.hpp>
#include <vist/query-builder/table.hpp>

using namespace vist::tsqb;

namespace {

struct Table1 {
	int column1;
	std::string column2;
	bool column3;
};

struct Table2 {
	int column1;
	std::string column2;
	bool column3;
	float column4;
	double column5;
};

Table table1 { "table1", Column("column1", &Table1::column1),
						 Column("column2", &Table1::column2),
						 Column("column3", &Table1::column3) };

Table table2 { "table2", Column("column1", &Table2::column1),
						 Column("column2", &Table2::column2),
						 Column("column3", &Table2::column3),
						 Column("column4", &Table2::column4),
						 Column("column5", &Table2::column5) };

Database database { "database", table1, table2 };

} // anonymous namespace

TEST(DatabaseTests, size)
{
	EXPECT_EQ(database.size(), 2);
}

TEST(DatabaseTests, get_name)
{
	EXPECT_EQ(database.name, "database");

	EXPECT_EQ(database.getTableName(Table1()), "table1");
	EXPECT_EQ(database.getTableName(Table2()), "table2");

	EXPECT_EQ(database.getColumnName(&Table1::column1), "table1.column1");
	EXPECT_EQ(database.getColumnName(&Table1::column2), "table1.column2");
	EXPECT_EQ(database.getColumnName(&Table1::column3), "table1.column3");

	EXPECT_EQ(database.getColumnName(&Table2::column1), "table2.column1");
	EXPECT_EQ(database.getColumnName(&Table2::column2), "table2.column2");
	EXPECT_EQ(database.getColumnName(&Table2::column3), "table2.column3");
	EXPECT_EQ(database.getColumnName(&Table2::column4), "table2.column4");
	EXPECT_EQ(database.getColumnName(&Table2::column5), "table2.column5");
}

TEST(DatabaseTests, get_names)
{
	auto columns = database.getColumnNames(&Table1::column1, &Table2::column3);
	auto tables = database.getTableNames(&Table1::column1, &Table2::column1, &Table2::column1);
	if (columns.size() == 2 && tables.size() == 2) {
		EXPECT_EQ(columns[0], "table1.column1");
		EXPECT_EQ(columns[1], "table2.column3");

		EXPECT_EQ(tables[0], "table1");
		EXPECT_EQ(tables[1], "table2");
	} else {
		EXPECT_TRUE(false);
	}
}
