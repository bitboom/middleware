ADD_OSQUERY_LIBRARY(osquery_tables_linux events/linux/passwd_changes.cpp
										 networking/linux/routes.cpp
										 networking/linux/socket_inode.cpp
										 networking/linux/port_inode.cpp
										 networking/linux/arp_cache.cpp
										 system/linux/kernel_modules.cpp
										 system/linux/processes.cpp
										 system/linux/users.cpp
										 system/linux/groups.cpp
										 system/linux/mounts.cpp)

ADD_OSQUERY_LIBRARY(osquery_tables networking/utils.cpp
								   networking/etc_hosts.cpp
								   utility/time.cpp
								   utility/crontab.cpp
								   system/last.cpp
								   system/bash_history.cpp
								   system/suid_bin.cpp
								   base.h)

ADD_OSQUERY_TEST(osquery_etc_hosts_tests networking/etc_hosts_tests.cpp)
