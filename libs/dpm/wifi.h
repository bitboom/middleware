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

#ifndef __CAPI_WIFI_POLICY_H__
#define __CAPI_WIFI_POLICY_H__

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
 * @see         dpm_context_acquire_wifi_policy()
 * @see         dpm_context_release_wifi_policy()
 * @see         dpm_wifi_is_profile_change_restricted()
 */
int dpm_wifi_set_profile_change_restriction(device_policy_manager_h handle, int enable);

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
 * @see         dpm_context_acquire_wifi_policy()
 * @see         dpm_context_release_wifi_policy()
 * @see         dpm_wifi_set_profile_change_restriction()
 */
int dpm_wifi_is_profile_change_restricted(device_policy_manager_h handle, int *is_enabled);

/**
 * @brief       Restricts network access based on the Wi-Fi network service set
 *              identifier(SSID).
 * @details     An administrator can use this API to restrict connecting to the Wi-Fi
 *              network based on the blocked network list.
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   enable true to enable the Wi-Fi network access restriction,
 *              false to disable the Wi-Fi network access restriction
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_manager_create()
 * @pre         Blocked network list must be added by dpm_wifi_add_ssid_to_blocklist()
 * @see         dpm_wifi_is_network_access_restricted()
 * @see         dpm_wifi_add_ssid_to_blocklist()
 * @see         dpm_wifi_remove_ssid_from_blocklist()
 */
int dpm_wifi_set_network_access_restriction(device_policy_manager_h handle, int enable);

/**
 * @brief       Checks whether the SSID-based Wi-Fi network access restriction is
 *              enabled.
 * @details     An administrator can use this API to check whether the SSID-based
 *              Wi-Fi network restriction is enabled.
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[out]  is_enabled true if restriction mode is enabled or
 *              false if restriction mode is disabled
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         handle must be created by dpm_manager_create()
 * @see         dpm_wifi_set_network_access_restriction()
 */
int dpm_wifi_is_network_access_restricted(device_policy_manager_h handle, int *is_enabled);

/**
 * @brief       Adds a service set identifier(SSID) to the list of blocked network.
 * @details     An administrator can use this API to add an SSID to the list of blocked
 *              networks, which prevents the user from connecting to it.
 *              The blocked network still appears in the Access Point list but is disabled.
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   ssid The SSID to block
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @retval      #DPM_ERROR_OUT_OF_MEMORY Too many SSIDs in blocked network list
 * @pre         handle must be created by dpm_manager_create()
 * @post        dpm_wifi_set_network_access_restriction() must be called
 *              when SSIDs in the blacklist are needed to get restricted
 * @see         dpm_wifi_remove_ssid_from_blocklist()
 * @see         dpm_wifi_set_network_access_restriction()
 */
int dpm_wifi_add_ssid_to_blocklist(device_policy_manager_h handle, const char* ssid);

/**
 * @brief       Removes a service set identifier(SSID) from the list of blocked
 *              networks.
 * @details     An administrator can use this API to remove an SSID from the list of
 *              blocked networks, which allows the user to connect to it.
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   ssid The SSID to be removed from the list of blocked networks
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_manager_create()
 * @see         dpm_wifi_add_ssid_to_blocklist()
 * @see         dpm_wifi_set_network_access_restriction()
 */
int dpm_wifi_remove_ssid_from_blocklist(device_policy_manager_h handle, const char* ssid);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //! __CAPI_WIFI_POLICY_H__
