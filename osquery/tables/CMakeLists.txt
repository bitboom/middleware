ADD_OSQUERY_LIBRARY(FALSE osquery_tables_linux events/linux/passwd_changes.cpp
											   events/linux/file_changes.cpp
											   events/linux/hardware_events.cpp
											   networking/linux/routes.cpp
											   networking/linux/process_open_sockets.cpp
											   networking/linux/arp_cache.cpp
											   system/linux/acpi_tables.cpp
											   system/linux/processes.cpp
											   system/linux/process_open_files.cpp
											   system/linux/shared_memory.cpp
											   system/linux/smbios_tables.cpp
											   system/linux/sysctl_utils.cpp
											   system/linux/users.cpp
											   system/linux/groups.cpp
											   system/linux/kernel_info.cpp
											   system/linux/kernel_modules.cpp
											   system/linux/memory_map.cpp
											   system/linux/mounts.cpp
											   system/linux/os_version.cpp
											   system/linux/pci_devices.cpp
											   system/linux/usb_devices.cpp)

ADD_OSQUERY_LIBRARY(TRUE osquery_tables  networking/utils.cpp
										 networking/etc_hosts.cpp  # TODO(sangwan.kwon): Change to FALSE)
										 networking/etc_services.cpp
										 networking/listening_ports.cpp
										 utility/time.cpp
										 utility/hash.cpp
										 utility/file.cpp
										 utility/osquery.cpp
										 system/crontab.cpp
										 system/smbios_utils.cpp
										 system/last.cpp
										 system/shell_history.cpp
										 system/suid_bin.cpp
										 system/system_controls.cpp
										 system/logged_in_users.cpp)

ADD_OSQUERY_TEST(FALSE osquery_etc_hosts_tests networking/etc_hosts_tests.cpp)
