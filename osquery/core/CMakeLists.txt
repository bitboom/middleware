ADD_OSQUERY_LIBRARY(TRUE osquery_core init.cpp
									  conversions.cpp
									  system.cpp
									  text.cpp
									  tables.cpp
									  flags.cpp
									  hash.cpp
									  watcher.cpp)

# TODO(Sangwan): Detach from core
ADD_OSQUERY_LIBRARY(TRUE osquery_test_util test_util.cpp)

FILE(GLOB OSQUERY_CORE_TESTS "tests/*.cpp")
ADD_OSQUERY_TEST(TRUE ${OSQUERY_CORE_TESTS})
