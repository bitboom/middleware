ADD_OSQUERY_LIBRARY(osquery_core init_osquery.cpp
								 sql.cpp
								 sqlite_util.cpp
								 system.cpp
								 test_util.cpp
								 text.cpp
								 tables.cpp
								 virtual_table.cpp
								 flags.cpp)

ADD_OSQUERY_TEST(osquery_status_tests status_tests.cpp)
ADD_OSQUERY_TEST(osquery_sql_tests sql_tests.cpp)
ADD_OSQUERY_TEST(osquery_sqlite_util_tests sqlite_util_tests.cpp)
ADD_OSQUERY_TEST(osquery_test_util_tests test_util_tests.cpp)
ADD_OSQUERY_TEST(osquery_text_tests text_tests.cpp)
ADD_OSQUERY_TEST(osquery_conversions_tests conversions_tests.cpp)
