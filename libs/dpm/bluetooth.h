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

#ifndef __CAPI_BLUETOOTH_POLICY_H__
#define __CAPI_BLUETOOTH_POLICY_H__

#include <dpm/device-policy-manager.h>

/**
 * @file bluetooth.h
 * @brief This file provides APIs to control bluetooth functionality such as
 *        bluetooth device or uuid restriction
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  CAPI_DPM_BLUETOOTH_POLICY_MODULE
 * @{
 */

/**
 * @brief       Adds MAC address to blacklist
 * @details     An administrator can use this API to add new MAC address to
 *              blacklist
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   address The MAC address that should be added
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_release_bluetooth_policy()
 * @see         dpm_bluetooth_remove_device_from_blacklist()
 */
int dpm_bluetooth_add_device_to_blacklist(device_policy_manager_h handle, const char* address);

/**
 * @brief       Removes MAC address from blacklist
 * @details     An administrator can use this API to remove MAC address from
 *              blacklist
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   address The MAC address that should be removed
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_release_bluetooth_policy()
 * @see         dpm_bluetooth_add_device_to_blacklist()
 */
int dpm_bluetooth_remove_device_from_blacklist(device_policy_manager_h handle, const char* address);

/**
 * @brief       Enables or disables device restriction of bluetooth
 * @details     An administrator can use this API to set whether the bluetooth
 *              is allowed or not by device restriction
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   enable If true, enable the bluetooth pairing restriction, if false, allow the bluetooth pairing
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_release_bluetooth_policy()
 * @see         dpm_bluetooth_is_device_restricted()
 */
int dpm_bluetooth_set_device_restriction(device_policy_manager_h handle, int enable);

/**
 * @brief       Gets the status of the bluetooth's device restriction
 * @details     An administrator can use this API to get the status of the
 *              bluetooth's device restriction. The device restricted bluetooth will be worked by MAC
 *              address blacklist
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[out]  is_enabled true if the bluetooth pairing restriction is enabled, else false
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @pre         handle must be created by dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_release_bluetooth_policy()
 * @see         dpm_bluetooth_set_device_restriction()
 */
int dpm_bluetooth_is_device_restricted(device_policy_manager_h handle, int *is_enabled);

/**
 * @brief       Adds UUID to blacklist
 * @details     An administrator can use this API to add new UUID to blacklist
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   uuid The UUID that should be added
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_release_bluetooth_policy()
 * @see         dpm_bluetooth_remove_uuid_from_blacklist()
 */
int dpm_bluetooth_add_uuid_to_blacklist(device_policy_manager_h handle, const char* uuid);

/**
 * @brief       Removes UUID from blacklist
 * @details     An administrator can use this API to remove UUID from blacklist
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   uuid The UUID that should be removed
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_release_bluetooth_policy()
 * @see         dpm_bluetooth_add_uuid_to_blacklist()
 */
int dpm_bluetooth_remove_uuid_from_blacklist(device_policy_manager_h handle, const char* uuid);

/**
 * @brief       Enables or disables UUID restriction of bluetooth
 * @details     An administrator can use this API to set whether the bluetooth
 *              is allowed or not by UUID restriction
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   enable If true, enable the bluetooth pairing restriction,
 *              if false, disable the bluetooth pairing restriction
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_release_bluetooth_policy()
 * @see         dpm_bluetooth_is_uuid_restricted()
 */
int dpm_bluetooth_set_uuid_restriction(device_policy_manager_h handle, int enable);

/**
 * @brief       Gets the bluetooth's UUID restriction status
 * @details     An administrator can use this API to get the bluetooth's UUID restriction status
 *              The UUID restricted bluetooth will be worked by UUID blacklist
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[out]  is_enabled true if the bluetooth pairing restriction is enabled, else false
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @pre         handle must be created by dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_acquire_bluetooth_policy()
 * @see         dpm_context_release_bluetooth_policy()
 * @see         dpm_bluetooth_set_uuid_restriction()
 */
int dpm_bluetooth_is_uuid_restricted(device_policy_manager_h handle, int *is_enabled);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //! __CAPI_BLUETOOTH_POLICY_H__
