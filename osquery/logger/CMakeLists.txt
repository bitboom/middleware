ADD_OSQUERY_LIBRARY(TRUE osquery_logger logger.cpp)
ADD_OSQUERY_LIBRARY(TRUE osquery_logger_plugins plugins/filesystem.cpp
												plugins/syslog.cpp)

FILE(GLOB OSQUERY_LOGGER_TESTS "tests/*.cpp")
ADD_OSQUERY_TEST(TRUE ${OSQUERY_LOGGER_TESTS})

file(GLOB OSQUERY_LOGGER_PLUGIN_TESTS "plugins/tests/*.cpp")
ADD_OSQUERY_TEST(TRUE ${OSQUERY_LOGGER_PLUGIN_TESTS})
