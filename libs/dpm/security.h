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

#ifndef __CAPI_SECURITY_POLICY_H__
#define __CAPI_SECURITY_POLICY_H__

#include <dpm/device-policy-manager.h>
#include <dpm/context.h>

/**
 * @file security.h
 * @brief This file provides APIs to control security functionality such as
 *        device encryption and screen lock.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  CAPI_DPM_SECURITY_POLICY_MODULE
 * @{
 */

/**
 * @partner
 * @brief       Locks device screen immediately.
 * @details     An administrator can use this API to lock the device screen
 *              immediately
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.lock
 * @param[in]   handle Device policy manager handle
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_security_lockout_screen(device_policy_manager_h handle);

/**
 * @partner
 * @brief       Encrypts or decrypts internal storage.
 * @details     An administrator can use this API to enable full device
 *              encryption, which includes device memory and internal SD card.
 *              Before calling this API, administrator must ensure that
 *              the device password is set to alphanumeric quality.
 *              The administrator can set an alphanumeric password by using
 *              dpm_set_password_quality() API
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.security
 * @param[in]   handle Device policy manager handle
 * @param[in]   encrypt true if encryption is required, false if decryption is
 *              required
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_security_is_internal_storage_encrypted()
 */
int dpm_security_set_internal_storage_encryption(device_policy_manager_h handle, int encrypt);

/**
 * @brief       Checks the internal storage encryption state.
 * @details     An administrator can use this API to check whether internal
 *              storage encryption is enabled.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]  is_encrypted true if internal storage is encrypted or being encrypted,
 *              else false.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_security_set_internal_storage_encryption()
 */
int dpm_security_is_internal_storage_encrypted(device_policy_manager_h handle, int *is_encrypted);

/**
 * @partner
 * @brief       Encrypts or decrypts external storage.
 * @details     An administrator can use this API to enable external SD card
 *              encryption. Before calling this API, administrator must
 *              ensure that the device password is set to alphanumeric quality.
 *              The administrator can set an alphanumeric password by using
 *              dpm_set_password_quality() API
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.security
 * @param[in]   handle Device policy manager handle
 * @param[in]   encrypt true if encryption is required, false if decryption is
 *              required
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_security_is_external_storage_encryped()
 */
int dpm_security_set_external_storage_encryption(device_policy_manager_h handle, int encrypt);

/**
 * @brief       Checks the external storage encryption state.
 * @details     An administrator can use this API to check whether external
 *              storage encryption is enabled.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]   is_encrypted true if external storage is encrypted or being encrypted,
 *              else false
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_security_set_external_storage_encryption()
 */
int dpm_security_is_external_storage_encrypted(device_policy_manager_h handle, int *is_encrypted);

/**
 * @brief       Enumeration for device wipe type.
 * @since_tizen 3.0
 */
typedef enum {
	DPM_SECURITY_WIPE_INTERNAL_STORAGE = (1 << 0), /**< Wipe internal memory */
	DPM_SECURITY_WIPE_EXTERNAL_STORAGE = (1 << 1),  /**< Wipe external memory */
} dpm_security_wipe_type_e;

/**
 * @partner
 * @brief       Wipes external memory, internal memory, or both selectively.
 * @details     Device Admin can use this API to wipe both SD card data
 *              and application data.
 *              Calling this API may require rebooting the device.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.wipe
 * @param[in]   handle Device policy manager handle
 * @param[in]   type The target storage for wipe
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_security_wipe_data(device_policy_manager_h handle, dpm_security_wipe_type_e type);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //! __CAPI_SECURITY_POLICY_H__
