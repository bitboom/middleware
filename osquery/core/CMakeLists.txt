ADD_OSQUERY_LIBRARY(osquery_core init_osquery.cpp
#								 sql.cpp // scheduler
								 sqlite_util.cpp
								 system.cpp
								 test_util.cpp
								 text.cpp)
#
ADD_OSQUERY_TEST(osquery_status_test status_tests.cpp)
#ADD_OSQUERY_TEST(sql_test sql_tests.cpp)
ADD_OSQUERY_TEST(osquery_sqlite_util_tests sqlite_util_tests.cpp)
ADD_OSQUERY_TEST(osquery_test_util_tests test_util_tests.cpp)
ADD_OSQUERY_TEST(osquery_text_tests text_tests.cpp)
#ADD_OSQUERY_TEST(osquery_conversions_tests conversions_tests.cpp)
