ADD_OSQUERY_LIBRARY(osquery_database db_handle.cpp
									 query.cpp
									 results.cpp)

ADD_OSQUERY_TEST(osquery_query_tests query_tests.cpp)
ADD_OSQUERY_TEST(osquery_db_handle_tests db_handle_tests.cpp)
ADD_OSQUERY_TEST(osquery_results_tests results_tests.cpp)
