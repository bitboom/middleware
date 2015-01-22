ADD_OSQUERY_LIBRARY(TRUE osquery_logger logger.cpp)
ADD_OSQUERY_LIBRARY(FALSE osquery_logger_plugins plugins/filesystem.cpp)

ADD_OSQUERY_TEST(FALSE osquery_logger_tests logger_tests.cpp)
