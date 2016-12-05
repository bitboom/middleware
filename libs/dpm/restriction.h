/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */

#ifndef __CAPI_RESTRICTION_POLICY_H__
#define __CAPI_RESTRICTION_POLICY_H__

#include <dpm/device-policy-manager.h>
#include <dpm/context.h>

/**
 * @file restriction.h
 * @brief This file provides APIs to control restriction functionality.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  CAPI_DPM_RESTRICTION_POLICY_MODULE
 * @{
 */

/**
 * @partner
 * @brief       Allows or disallows the use of camera.
 * @details     An administrator can use this API to set whether the use of camera
 *              is allowed or not.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.camera
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow the use of camera, if false, disallow the use of camera
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_camera_state()
 */
int dpm_restriction_set_camera_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Check whether the use of camera is allowed or not.
 * @details     An administrator can use this API to check whether the use of camera
 *              is allowed or not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the use of camera is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_camera_state()
 */
int dpm_restriction_get_camera_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows the use of microphone.
 * @details     An administrator can use this API to set whether the use of microphone
 *              is allowed or not.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.microphone
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow the use of microphone,
 *              if false, disallow the use of microphone
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_microphone_state()
 */
int dpm_restriction_set_microphone_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the use of microphone is allowed or not.
 * @details     An administrator can use this API to check whether the use of microphone
 *              is allowed of not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the use of microphone is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_microphone_state()
 */
int dpm_restriction_get_microphone_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to change the location state.
 * @details     An administrator can use this API to allow or disallow user to change
 *              the location state.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.location
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow user to change the location state, if false, disallow
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_location_state()
 */
int dpm_restriction_set_location_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Check whether user is allowed to change location state or not.
 * @details     An administrator can use this API to check whether user is allowed to change
 *              the location state or not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the location state change is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_location_state()
 */
int dpm_restriction_get_location_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to use usb mass storage.
 * @details     An administrator can use this API to set whether the usb mass
 *              storage is allowed or not.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.storage
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow user to use the external storages, if false, disallow
 *              the external storage
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_storage_get_external_storage_state()
 */
int dpm_restriction_set_external_storage_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the use of external storage is allowed or not.
 * @details     An administrator can use this API to check whether the use of external storage is
 *              allowed or not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the use of external storage is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_storage_set_external_storage_state()
 */
int dpm_restriction_get_external_storage_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to access the clipboard.
 * @details     An administrator can use this API to set whether the clipboard access.
 *              is allowed or not
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.clipboard
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow the clipboard access, if false, disallow the clipboard access.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_clipboard_state()
 */
int dpm_restriction_set_clipboard_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the clipboard access is allowed or not.
 * @details     An administrator can use this API to check whether the clipboard access
 *              is allowed or not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the access is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_clipboard_state()
 */
int dpm_restriction_get_clipboard_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows the usb debugging.
 * @details     An administrator can use this API to set whether the usb debugging
 *              is allowed or not.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.debugging
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow the usb debugging, if false, disallow the usb
 *              debugging
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_usb_debugging_state()
 */
int dpm_restriction_set_usb_debugging_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the usb debugging is allowed or not.
 * @details     An administrator can use this API to check whether the usb debugging
 *              is allowed or not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]   is_allowed true if the usb debugging is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_usb_debugging_state()
 */
int dpm_restriction_get_usb_debugging_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to change the Wi-Fi state.
 * @details     An administrator can use this API to allow or disallow user to
 *              change the Wi-Fi state. If it is disallowed, user does not have UI
 *              access to change the state.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.wifi
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow user to change Wi-Fi state,
 *              if false, disallow user to change Wi-Fi state.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_wifi_state()
 */
int dpm_restriction_set_wifi_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the Wi-Fi state change is allowed or not.
 * @details     An administrator can use this API to check whether user is
 *              allowed to change Wi-Fi state or not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the change is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_wifi_state()
 */
int dpm_restriction_get_wifi_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to change Wi-Fi hotspot state change.
 * @details     An administrator can use this API to allow or disallow user to change Wi-Fi
 *              hotspot state. When it is disallowed, the UI is grayed out so user cannot
 *              change Wi-Fi hotspot state.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.wifi
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow user to change Wi-Fi hostspot state,
 *              if false, disallow user to change Wi-Fi hotspot state.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_wifi_hotspot_state()
 */
int dpm_restriction_set_wifi_hotspot_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the the Wi-Fi hotspot state change is allowed or not.
 * @details     An administrator can use this API to check whether user is allowed to change
 *              Wi-Fi hotspot state or not.
 *              If the Wi-Fi hotspot state change is disallowed, the UI is grayed out so user can not
 *              change its state.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the state change is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_wifi_hotspot_state()
 */
int dpm_restriction_get_wifi_hotspot_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to change the bluetooth tethering state.
 * @details     An administrator can use this API to allow of disallow user
 *              to chanage the bluetooth tethering state.
 *              When disallowed, the UI is grayed out so user cannot change the state.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.bluetooth
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow user to change the bluetooth tethering state,
 *              if false, disallow user to change the bluetooth tethering state.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_bluetooth_tethering_state()
 */
int dpm_restriction_set_bluetooth_tethering_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the bluetooth tethering state change is allowed.
 * @details     An administrator can use this API to check whether user is allowed
 *              to change bluetooth tethering state.
 *              is allowed or not.
 *              If the bluetooth tethering state change is disallowed, the UI is grayed out
 *              so user can not change its state.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the change is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_bluetooth_tethering_state()
 */
int dpm_restriction_get_bluetooth_tethering_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to change USB tethering settings.
 * @details     An administrator can use this API to allow or disallow user
 *              to change USB tethering settings.
 *              When disallowed, the UI is grayed out so user cannot change the state.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.usb
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow USB tethering state change,
 *              if false, disallow USB tethering state change.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_usb_tethering_state()
 */
int dpm_restriction_set_usb_tethering_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the USB tethering state change is allowed.
 * @details     An administrator can use this API to check whether the USB tethering state change
 *              is allowed or not.
 *              If the USB tethering state change is disallowed, the UI is grayed out
 *              so user can not change its state.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the change is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_usb_tethering_state()
 */
int dpm_restriction_get_usb_tethering_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to change the bluetooth state.
 * @details     An administrator can use this API to allow or disallow user
 *              to change the bluetooth state.
 *              When disallowed, the UI is grayed out so user cannot change the state.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.bluetooth
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow user to change bluetooth state,
 *              if false, disallow user to change bluetooth state.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_bluetooth_mode_change_state()
 */
int dpm_restriction_set_bluetooth_mode_change_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the the bluetooth state change is allowed of not.
 * @details     An administrator can use this API to check whether the bluetooth state change
 *              is allowed or not.
 *              If the bluetooth state change is disallowed, the UI is grayed out
 *              so user can not change its state.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the change is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_bluetooth_mode_change_state()
 */
int dpm_restriction_get_bluetooth_mode_change_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows the bluetooth desktop connectivity.
 * @details     An administrator can use this API to allow or disallow the bluetooth
 *              desktop connectivity.
 *              If the bluetooth desktop connectivity is disallowed, the UI is grayed out
 *              so user can not
 *              change its state.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.bluetooth
 * @param[in]   handle Device policy manager handle
 * @param[out]  allow If true, allow the bluetooth desktop connectivity,
 *              if false, disallow the bluetooth desktop connectivitiy
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_bluetooth_desktop_connectivity_state()
 */
int dpm_restriction_set_bluetooth_desktop_connectivity_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the the Bluetooth desktop connectivity is allowed or not.
 * @details     An administrator can use this API to check whether the Bluetooth desktop
 *              connectivity is allowed or not.
 *              If the Bluetooth desktop connectivity is disallowed, the UI is grayed out
 *              so user can not change its state.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the connectivity is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_bluetooth_desktop_connectivity_state()
 */
int dpm_restriction_get_bluetooth_desktop_connectivity_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to change the bluetooth pairing.
 * @details     An administrator can use this API to allow or disallow the bluetooth pairing.
 *              If the bluetooth pairing is disallowed, the UI is grayed out
 *              so user can not change its state.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.bluetooth
 * @param[in]   handle Device policy manager handle
 * @param[out]  allow If true, allow the bluetooth pairing, if false, disallow the bluetooth pairing.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_get_bluetooth_pairing_state()
 */
int dpm_restriction_set_bluetooth_pairing_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the the bluetooth pairing is allowed or not.
 * @details     An administrator can use this API to check whether the bluetooth
 *              pairing is allowed or not.
 *              If the bluetooth pairing is disallowed, the UI is grayed out
 *              so user can not change its state.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the pairing is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_restriction_set_bluetooth_pairing_state()
 */
int dpm_restriction_get_bluetooth_pairing_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows user to use of SMS or text messaging.
 * @details     An administrator can disable the text messaging capability
 *              without any user interaction
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.message
 * @param[in]   handle Device policy manager handle
 * @param[in]   sim_id SIM identifier
 * @param[in]   allow If true, allow the use of SMS or text messaging,
 *              if false, disallow the use of SMS or text messaging.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_restriction_get_messaging_state()
 */
int dpm_restriction_set_messaging_state(device_policy_manager_h handle, const char *sim_id, int allow);

/**
 * @brief       Checks whether the text messaging is allowed or not.
 * @details     An administrator can use this API to check whether text messaging capability
 *              is enabled or not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[in]   sim_id SIM identifier
 * @param[out]  is_allowed true if the messaging is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_restriction_set_messaging_state()
 */
int dpm_restriction_get_messaging_state(device_policy_manager_h handle, const char *sim_id, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows the access to POP or IMAP email.
 * @details     An administrator can disable the email capability without any user interaction
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.email
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If ture, allow POP or IMAP email, if false, disallow POP or IMAP email.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_restriction_get_popimap_email_state()
 */
int dpm_restriction_set_popimap_email_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the access to POP or IMAP email is allowed or not.
 * @details     An administrator can use this API to check the access to POP or IMAP email
 *              is allowed or not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the POP or IMAP email is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_restriction_set_popimap_email_state()
 */
int dpm_restriction_get_popimap_email_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @partner
 * @brief       Allows or disallows the use of web browser.
 * @details     An administrator can allows or disallow the use of web browser without
 *              any user interaction
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.browser
 * @param[in]   handle Device policy manager handle
 * @param[in]   allow If true, allow the use of web browser,
 *              if false, disallow the use of web browser.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_restriction_get_browser_state()
 */
int dpm_restriction_set_browser_state(device_policy_manager_h handle, int allow);

/**
 * @brief       Checks whether the use of web browser is allowed or not.
 * @details     An administrator can use this API to check whether the use of web browser
 *              is allowed or not.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_allowed true if the use of web browser is allowed, false otherwise.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_restriction_set_browser_state()
 */
int dpm_restriction_get_browser_state(device_policy_manager_h handle, int *is_allowed);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CAPI_RESTRICTION_POLICY_H__ */
