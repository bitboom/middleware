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

ADD_OSQUERY_TEST(TRUE osquery_flags_tests flags_tests.cpp)
ADD_OSQUERY_TEST(TRUE osquery_hash_tests hash_tests.cpp)
ADD_OSQUERY_TEST(TRUE osquery_status_tests status_tests.cpp)
ADD_OSQUERY_TEST(TRUE osquery_tables_tests tables_tests.cpp)
ADD_OSQUERY_TEST(TRUE osquery_text_tests text_tests.cpp)
ADD_OSQUERY_TEST(TRUE osquery_conversions_tests conversions_tests.cpp)
