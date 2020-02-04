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
#include <vist/query-builder/table.hpp>

using namespace vist::tsqb;

namespace {

struct Table1 {
	int column1;
	std::string column2;
	bool column3;

	inline static Column Column1 = { "column1", &Table1::column1 };
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

} // anonymous namespace

TEST(QueryBuilderTableTests, size)
{
	EXPECT_EQ(table1.size(), 3);
	EXPECT_EQ(table2.size(), 5);
}

TEST(QueryBuilderTableTests, get_name)
{
	EXPECT_EQ(table1.getName(), "table1");
	EXPECT_EQ(table2.name, "table2");

	EXPECT_EQ(table1._getColumnName(Table1::Column1), "column1");
	EXPECT_EQ(table1.getColumnName(&Table1::column1), "column1");
	EXPECT_EQ(table1.getColumnName(&Table1::column2), "column2");
	EXPECT_EQ(table1.getColumnName(&Table1::column3), "column3");

	EXPECT_EQ(table2.getColumnName(&Table2::column1), "column1");
	EXPECT_EQ(table2.getColumnName(&Table2::column2), "column2");
	EXPECT_EQ(table2.getColumnName(&Table2::column3), "column3");
	EXPECT_EQ(table2.getColumnName(&Table2::column4), "column4");
	EXPECT_EQ(table2.getColumnName(&Table2::column5), "column5");
}

TEST(QueryBuilderTableTests, get_names)
{
	EXPECT_EQ(table1.getColumnNames().size(), 3);
	EXPECT_EQ(table2.getColumnNames().size(), 5);
}

TEST(QueryBuilderTableTests, compare)
{
	EXPECT_TRUE(table1.compare(Table1()));
}
