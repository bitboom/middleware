ADD_OSQUERY_LIBRARY(TRUE osquery_filesystem filesystem.cpp
 											globbing.cpp)

ADD_OSQUERY_LIBRARY(TRUE osquery_filesystem_linux linux/proc.cpp
												  linux/mem.cpp)

FILE(GLOB OSQUERY_FILESYSTEM_TESTS "tests/*.cpp")
ADD_OSQUERY_TEST(TRUE ${OSQUERY_FILESYSTEM_TESTS})
