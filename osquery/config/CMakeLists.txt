ADD_OSQUERY_LIBRARY(osquery_config config.cpp
								   plugins/filesystem.cpp)

ADD_OSQUERY_TEST(osquery_config_tests config_tests.cpp)
