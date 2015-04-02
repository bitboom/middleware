ADD_OSQUERY_LIBRARY(TRUE osquery_config config.cpp)
ADD_OSQUERY_LIBRARY(FALSE osquery_config_plugins update.cpp
												 plugins/filesystem.cpp)

FILE(GLOB OSQUERY_CONFIG_TESTS "tests/*.cpp")
