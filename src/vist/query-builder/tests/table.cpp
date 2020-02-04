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

#include "schema.hpp"

using namespace vist::tsqb;
using namespace vist::test;

TEST(QueryBuilderTableTests, size)
{
	EXPECT_EQ(table1.size(), 3);
	EXPECT_EQ(table2.size(), 5);
}

TEST(QueryBuilderTableTests, get_name)
{
	EXPECT_EQ(table1.getName(), "table1");
	EXPECT_EQ(table2.name, "table2");

	EXPECT_EQ(table1.getColumnName(Table1::Column1), "column1");
	EXPECT_EQ(table1.getColumnName(Table1::Column2), "column2");
	EXPECT_EQ(table1.getColumnName(Table1::Column3), "column3");

	EXPECT_EQ(table2.getColumnName(Table2::Column1), "column1");
	EXPECT_EQ(table2.getColumnName(Table2::Column2), "column2");
	EXPECT_EQ(table2.getColumnName(Table2::Column3), "column3");
	EXPECT_EQ(table2.getColumnName(Table2::Column4), "column4");
	EXPECT_EQ(table2.getColumnName(Table2::Column5), "column5");
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
