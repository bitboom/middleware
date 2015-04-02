ADD_OSQUERY_LIBRARY(TRUE osquery_database db_handle.cpp
										  query.cpp
										  results.cpp)

FILE(GLOB OSQUERY_DATABASE_TESTS "tests/*.cpp")
ADD_OSQUERY_TEST(TRUE ${OSQUERY_DATABASE_TESTS})
