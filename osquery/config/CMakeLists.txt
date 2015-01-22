ADD_OSQUERY_LIBRARY(TRUE osquery_config config.cpp)
ADD_OSQUERY_LIBRARY(FALSE osquery_config_plugins plugins/filesystem.cpp)

ADD_OSQUERY_TEST(FALSE osquery_config_tests config_tests.cpp)
