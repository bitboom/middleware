ADD_OSQUERY_LIBRARY(TRUE osquery_filesystem filesystem.cpp)

ADD_OSQUERY_LIBRARY(TRUE osquery_filesystem_linux linux/proc.cpp
												  linux/mem.cpp)

ADD_OSQUERY_TEST(TRUE osquery_filesystem_tests filesystem_tests.cpp)
