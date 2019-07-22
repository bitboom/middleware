/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __CAPI_DPM_WIFI_POLICY_H__
#define __CAPI_DPM_WIFI_POLICY_H__

#include <stdbool.h>

#include <dpm/device-policy-manager.h>

/**
 * @file wifi.h
 * @brief This file provides APIs to control wifi policy
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  CAPI_DPM_WIFI_POLICY_MODULE
 * @{
 */

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
 * @see         dpm_wifi_get_state()
 */
int dpm_wifi_set_state(device_policy_manager_h handle, bool allow);

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
 * @see         dpm_wifi_set_state()
 */
int dpm_wifi_get_state(device_policy_manager_h handle, bool *is_allowed);

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
 * @see         dpm_wifi_get_hotspot_state()
 */
int dpm_wifi_set_hotspot_state(device_policy_manager_h handle, bool allow);

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
 * @see         dpm_wifi_set_hotspot_state()
 */
int dpm_wifi_get_hotspot_state(device_policy_manager_h handle, bool *is_allowed);

/**
 * @brief       Allows or disallows user to modify some Wi-Fi profiles of network settings.
 * @details     An administrator can use this API to allow or disallow users to modify selected
 *              Wi-Fi profiles like static ip configuration, proxy settings, security type
 *              and others. When this policy is in effect the user is only allowed to
 *              modify only the username, password, anonymous identity, and wep key.
 *              In addition, the user cannot remove the network. When false, the user can
 *              modify all Wi-fi network profiles normally and also remove it.
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   enable true to enable restriction mode for wifi profile changes, else false
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_acquire_wifi_policy()
 * @see         dpm_manager_create()
 * @see         dpm_wifi_is_profile_change_restricted()
 */
int dpm_wifi_set_profile_change_restriction(device_policy_manager_h handle, bool enable);

/**
 * @brief       Checks if the user is allowed to modify certain Wi-Fi profiles.
 * @details     An administrator can use this API to check whether the user is
 *              allowed to modify Wi-Fi profiles. The user is restricted in modifying
 *              Wi-Fi profiles if at least one administrator has set the value to TRUE.
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[out]  is_enabled true if one or more administrators enabled restriction
 *              false if user can change all Wi-Fi profiles
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         handle must be created by dpm_context_acquire_wifi_policy()
 * @see         dpm_manager_create()
 * @see         dpm_wifi_set_profile_change_restriction()
 */
int dpm_wifi_is_profile_change_restricted(device_policy_manager_h handle, bool *is_enabled);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //! __CAPI_DPM_WIFI_POLICY_H__
