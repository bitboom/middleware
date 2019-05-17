ADD_OSQUERY_LIBRARY(osquery_events events.cpp
								   linux/inotify.cpp)

ADD_OSQUERY_TEST(osquery_events_tests events_tests.cpp)
ADD_OSQUERY_TEST(osquery_events_database_tests events_database_tests.cpp)
ADD_OSQUERY_TEST(osquery_inotify_tests linux/inotify_tests.cpp)
