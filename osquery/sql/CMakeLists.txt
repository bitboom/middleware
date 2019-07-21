ADD_OSQUERY_LIBRARY(TRUE osquery_sql sql.cpp)

ADD_OSQUERY_LIBRARY(TRUE osquery_sql_internal sqlite_util.cpp
											  virtual_table.cpp)

FILE(GLOB OSQUERY_SQL_TESTS "tests/*.cpp")
ADD_OSQUERY_TEST(TRUE ${OSQUERY_SQL_TESTS})
