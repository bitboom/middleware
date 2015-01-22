ADD_OSQUERY_LIBRARY(TRUE osquery_sql sql.cpp
									 sqlite_util.cpp
									 virtual_table.cpp)

ADD_OSQUERY_TEST(TRUE osquery_sql_test sql_tests.cpp)
ADD_OSQUERY_TEST(TRUE osquery_sqlite_util_tests sqlite_util_tests.cpp)
ADD_OSQUERY_TEST(TRUE osquery_virtual_table_tests virtual_table_tests.cpp)
