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

ADD_OSQUERY_LIBRARY(osquery_core flags.cpp
								 tables.cpp
								 init.cpp
								 query.cpp
								 system.cpp
								 watcher.cpp
								 database/database.cpp
								 database/in_memory_database.cpp
								 plugins/logger.cpp
								 plugins/plugin.cpp
								 plugins/sql.cpp
								 posix/initializer.cpp
								 sql/column.cpp
								 sql/diff_results.cpp
								 sql/query_data.cpp
								 sql/query_performance.cpp
								 sql/row.cpp
								 sql/scheduled_query.cpp
								 sql/table_rows.cpp)

FILE(GLOB OSQUERY_CORE_TESTS "tests/*.cpp")
FILE(GLOB OSQUERY_CORE_POSIX_TESTS "tests/posix/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_SQL_TESTS}
				 ${OSQUERY_CORE_POSIX_TESTS}
				 ${OSQUERY_CORE_PLUGINS_TESTS})
