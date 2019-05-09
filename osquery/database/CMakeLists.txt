ADD_OSQUERY_LIBRARY(osquery_database
  db_handle.cpp
)

ADD_OSQUERY_TEST(db_handle_tests db_handle_tests.cpp)
