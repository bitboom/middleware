ADD_OSQUERY_LIBRARY(osquery_tables_linux events/linux/passwd_changes.cpp
										 networking/linux/routes.cpp
										 system/linux/kernel_modules.cpp
										 system/linux/processes.cpp
										 system/linux/users.cpp
										 system/linux/groups.cpp)

ADD_OSQUERY_LIBRARY(osquery_tables networking/utils.cpp
								   networking/etc_hosts.cpp
								   utility/time.cpp
								   system/last.cpp
								   base.h)

ADD_OSQUERY_TEST(osquery_etc_hosts_tests networking/etc_hosts_tests.cpp)
