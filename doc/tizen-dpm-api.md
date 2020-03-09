```
+--------------------------------+----------------------+----------------------------------------------------------+
|            Policy Name         |     Policy state     |                       Query API                          |
+--------------------------------+----------------------+----------------------------------------------------------+
| wifi                           | allowed / disallowed | dpm_restriction_get_wifi_state                           |
| wifi-hotspot                   | allowed / disallowed | dpm_restriction_get_wifi_hotspot                         |
| wifi-profile-change            | allowed / disallowed | dpm_wifi_is_profile_change_restricted                    |
| wifi-ssid-restriction          | allowed / disallowed | dpm_wifi_is_network_access_restricted                    |
| bluetooth                      | allowed / disallowed | dpm_restriction_get_bluetooth_mode_change_state          |
| bluetooth-tethering            | allowed / disallowed | dpm_restriction_get_bluetooth_tethering_state            |
| bluetooth-desktop-connectivity | allowed / disallowed | dpm_restriction_get_bluetooth_desktop_connectivity_state |
| bluetooth-pairing              | allowed / disallowed | dpm_restriction_get_bluetooth_pairing_state              |
| bluetooth-device-restriction   | allowed / disallowed | dpm_bluetooth_is_device_restricted                       |
| bluetooth-uuid-restriction     | allowed / disallowed | dpm_bluetooth_is_uuid_restricted                         |
| usb                            | allowed / disallowed | dpm_restriction_get_usb_state                            |
| usb-tethering                  | allowed / disallowed | dpm_restriction_get_usb_tethering_state                  |
| usb-debugging                  | allowed / disallowed | dpm_restriction_get_usb_debugging_state                  |
| settings-changes               | allowed / disallowed | dpm_restriction_get_setting_changes_state                |
| external-storage               | allowed / disallowed | dpm_restriction_get_external_storage_state               |
| camera                         | allowed / disallowed | dpm_restriction_get_camera_state                         |
| clipboard                      | allowed / disallowed | dpm_restriction_get_clipboard_state                      |
| location                       | allowed / disallowed | dpm_restriction_get_location_state                       |
| microphone                     | allowed / disallowed | dpm_restriction_get_microphone_state                     |
+--------------------------------+----------------------+----------------------------------------------------------+
```
