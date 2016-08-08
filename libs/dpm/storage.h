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

#ifndef __CAPI_STORAGE_POLICY_H__
#define __CAPI_STORAGE_POLICY_H__

#include <dpm/device-policy-manager.h>

/**
 * @file storage.h
 * @brief This file provides APIs to control storage
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  CAPI_DPM_STORAGE_POLICY_MODULE
 * @{
 */

/**
 * @brief       Enumeration for device wipe type
 * @since_tizen 3.0
 */
typedef enum {
	WIPE_INTERNAL_STORAGE = (1 << 0), /**< Wipe internal memory. */
	WIPE_EXTERNAL_STORAGE = (1 << 1)  /**< Wipe external memory. */
} dpm_wipe_type_e;

/**
 * @brief       The storage policy handle
 * @since_tizen 3.0
 * @see         dpm_context_acquire_storage_policy()
 * @see         dpm_context_release_storage_policy()
 */
typedef void* dpm_storage_policy_h;

/**
 * @brief       Acquires the storage policy handle.
 * @details     This API acquires storage policy handle required to enforce
 *              the storage policies.
 * @since_tizen 3.0
 * @param[in]   handle The device policy context handle
 * @return      Storage policy handle on success, otherwise NULL
 * @remark      The specific error code can be obtained by using the
 *              get_last_result() method. Error codes are described in
 *              exception section.
 * @exception   #DPM_ERROR_NONE No error
 * @exception   #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception   #DPM_ERROR_TIMED_OUT Time out
 * @see         dpm_context_release_storage_policy()
 * @see         get_last_result()
 */
dpm_storage_policy_h dpm_context_acquire_storage_policy(dpm_context_h handle);

/**
 * @brief       Releases the storage policy handle.
 * @details     This API must be called if interaction with the device
 *              policy manager is no longer required.
 * @since_tizen 3.0
 * @param[in]   handle The device policy context
 * @param[in]   handle The storage policy handle
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @pre         The handle must be created by dpm_context_acquire_storage_policy().
 * @see         dpm_context_acquire_storage_policy()
 */
int dpm_context_release_storage_policy(dpm_context_h context, dpm_storage_policy_h handle);

/**
 * @brief       API to selectively wipe external memory, internal memory,
 *              or both.
 * @details     Device Admin can use this API to wipe both SD card data
 *              and application data.
 *              Calling this API may require rebooting the device.
 * @since_tizen 3.0
 * @param[in]   handle Storage policy handle
 * @param[in]   type DPM_WIPE_INTERNAL_STORAGE or DPM_WIPE_EXTERNAL_STORAGE
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_acquire_storage_policy()
 * @see         dpm_context_acquire_storage_policy()
 * @see         dpm_context_release_storage_policy()
 */
int dpm_storage_wipe_data(dpm_storage_policy_h handle, const dpm_wipe_type_e type);

#ifdef __cplusplus
}
#endif

#endif //__CAPI_STORAGE_POLICY_H__
