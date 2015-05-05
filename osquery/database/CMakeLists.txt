ADD_OSQUERY_LIBRARY(TRUE osquery_database database.cpp)


ADD_OSQUERY_LIBRARY(TRUE osquery_database_internal db_handle.cpp
												   query.cpp)

FILE(GLOB OSQUERY_DATABASE_TESTS "tests/*.cpp")
ADD_OSQUERY_TEST(TRUE ${OSQUERY_DATABASE_TESTS})
