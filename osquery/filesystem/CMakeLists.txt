ADD_OSQUERY_LIBRARY(osquery_filesystem filesystem.cpp)

ADD_OSQUERY_LIBRARY(osquery_filesystem_linux linux/proc.cpp
											 linux/mem.cpp)

ADD_OSQUERY_TEST(osquery_filesystem_tests filesystem_tests.cpp)
