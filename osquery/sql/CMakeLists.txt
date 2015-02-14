ADD_OSQUERY_LIBRARY(TRUE osquery_sql sql.cpp)

ADD_OSQUERY_LIBRARY(FALSE osquery_sql_internal sqlite_util.cpp
											   virtual_table.cpp)

ADD_OSQUERY_TEST(TRUE osquery_sql_tests sql_tests.cpp)
ADD_OSQUERY_TEST(FALSE osquery_sqlite_util_tests sqlite_util_tests.cpp)
ADD_OSQUERY_TEST(FALSE osquery_virtual_table_tests virtual_table_tests.cpp)

