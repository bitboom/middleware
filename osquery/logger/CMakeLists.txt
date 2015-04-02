ADD_OSQUERY_LIBRARY(TRUE osquery_logger logger.cpp)
ADD_OSQUERY_LIBRARY(FALSE osquery_logger_plugins plugins/filesystem.cpp)

FILE(GLOB OSQUERY_LOGGER_TESTS "tests/*.cpp")
ADD_OSQUERY_TEST(FALSE ${OSQUERY_LOGGER_TESTS})
