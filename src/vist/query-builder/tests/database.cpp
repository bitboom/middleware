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

#include "schema.hpp"

using namespace vist::tsqb;
using namespace vist::test;

TEST(QueryBuilderDatabaseTests, size)
{
	EXPECT_EQ(database.size(), 2);
}

TEST(QueryBuilderDatabaseTests, get_name)
{
	EXPECT_EQ(database.name, "database");

	EXPECT_EQ(database.getTableName(Table1()), "table1");
	EXPECT_EQ(database.getTableName(Table2()), "table2");

	EXPECT_EQ(database.getColumnName(Table1::Column2), "table1.column2");
	EXPECT_EQ(database.getColumnName(Table1::Column3), "table1.column3");

	EXPECT_EQ(database.getColumnName(Table2::Column1), "table2.column1");
	EXPECT_EQ(database.getColumnName(Table2::Column2), "table2.column2");
	EXPECT_EQ(database.getColumnName(Table2::Column3), "table2.column3");
	EXPECT_EQ(database.getColumnName(Table2::Column4), "table2.column4");
	EXPECT_EQ(database.getColumnName(Table2::Column5), "table2.column5");
}

TEST(QueryBuilderDatabaseTests, get_names)
{
	auto columns = database.getColumnNames(Table1::Column1, Table2::Column3);
	auto tables = database.getTableNames(Table1::Column1, Table2::Column1, Table2::Column1);
	if (columns.size() == 2 && tables.size() == 2) {
		EXPECT_EQ(columns[0], "table1.column1");
		EXPECT_EQ(columns[1], "table2.column3");

		EXPECT_EQ(tables[0], "table1");
		EXPECT_EQ(tables[1], "table2");
	} else {
		EXPECT_TRUE(false);
	}
}
