ADD_OSQUERY_LIBRARY(TRUE osquery_events events.cpp)
ADD_OSQUERY_LIBRARY(FALSE osquery_events_linux linux/inotify.cpp
											   linux/udev.cpp)

ADD_OSQUERY_TEST(TRUE osquery_events_tests events_tests.cpp)
ADD_OSQUERY_TEST(TRUE osquery_events_database_tests events_database_tests.cpp)
ADD_OSQUERY_TEST(FALSE osquery_inotify_tests linux/inotify_tests.cpp)

