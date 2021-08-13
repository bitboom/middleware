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
#pragma once

#include <vist/query-builder.hpp>

using namespace vist::tsqb;

namespace vist {
namespace test {

struct Table1 {
	int column1;
	std::string column2;
	bool column3;

	DECLARE_COLUMN(Column1, "column1", &Table1::column1);
	DECLARE_COLUMN(Column2, "column2", &Table1::column2);
	DECLARE_COLUMN(Column3, "column3", &Table1::column3);
};

struct Table2 {
	int column1;
	std::string column2;
	bool column3;
	float column4;
	double column5;

	DECLARE_COLUMN(Column1, "column1", &Table2::column1);
	DECLARE_COLUMN(Column2, "column2", &Table2::column2);
	DECLARE_COLUMN(Column3, "column3", &Table2::column3);
	DECLARE_COLUMN(Column4, "column4", &Table2::column4);
	DECLARE_COLUMN(Column5, "column5", &Table2::column5);
};

DECLARE_TABLE(table1, "table1", Table1::Column1, Table1::Column2, Table1::Column3);
DECLARE_TABLE(table2, "table2", Table2::Column1,
			  Table2::Column2,
			  Table2::Column3,
			  Table2::Column4,
			  Table2::Column5);

DECLARE_DATABASE(database, "database", table1, table2);

} // namespace test
} // namespace vist
