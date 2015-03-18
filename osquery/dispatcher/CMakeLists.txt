ADD_OSQUERY_LIBRARY(TRUE osquery_dispatcher dispatcher.cpp
											scheduler.cpp)

ADD_OSQUERY_TEST(TRUE osquery_dispatcher_tests dispatcher_tests.cpp)
