ADD_OSQUERY_LIBRARY(osquery_tables_linux events/linux/passwd_changes.cpp
										 events/linux/hardware_events.cpp
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
								   utility/hash.cpp
								   utility/file.cpp
								   system/last.cpp
								   system/shell_history.cpp
								   system/suid_bin.cpp
								   system/logged_in_users.cpp)

ADD_OSQUERY_TEST(osquery_etc_hosts_tests networking/etc_hosts_tests.cpp)
