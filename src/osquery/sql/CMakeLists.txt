#  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License

ADD_OSQUERY_LIBRARY(osquery_sql dynamic_table_row.cpp
								sqlite_encoding.cpp
								sqlite_hashing.cpp
								sqlite_util.cpp
								sql.cpp
								sqlite_filesystem.cpp
								sqlite_math.cpp
								sqlite_string.cpp
								virtual_table.cpp
								virtual_sqlite_table.cpp)

FILE(GLOB OSQUERY_SQL_TESTS "tests/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_SQL_TESTS})
