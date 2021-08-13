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

#ifndef __CAPI_ADMINISTRATION_POLICY_H__
#define __CAPI_ADMINISTRATION_POLICY_H__

#include <sys/types.h>

#include <dpm/device-policy-manager.h>

/**
 * @file administration.h
 * @brief This file provides APIs to register/deregister the device admin client
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  CAPI_DPM_ADMINISTRATION_POLICY_MODULE
 * @{
 */

/**
 * @brief       Registers device policy admin client.
 * @details     This API is used to register the device policy admin client to
 *              the device policy manager. The device policy admin client must be
 *              registered to use the device administration APIs.
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   name The name of the device policy admin package
 * @param[in]   uid The uid of the device policy admin package
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid package name
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_create()
 * @see         dpm_context_acquire_admin_policy()
 * @see         dpm_context_release_admin_policy()
 * @see         dpm_admin_deregister_client()
 */
int dpm_admin_register_client(device_policy_manager_h handle, const char* name, uid_t uid);

/**
 * @brief       Deregisters the device policy admin client.
 * @details     This API is used to deregister the Device Policy Client from
 *              the device policy manager.
 * @since_tizen 3.0
 * @param[in]   handle The device policy manager handle
 * @param[in]   name The name of the device policy admin package
 * @param[in]   uid The uid of the device policy admin package
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMEOUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid package name
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         handle must be created by dpm_context_create()
 * @see         dpm_context_acquire_admin_policy()
 * @see         dpm_context_release_admin_policy()
 * @see         dpm_admin_register_client()
 */
int dpm_admin_deregister_client(device_policy_manager_h handle, const char* name, uid_t uid);

#ifdef __cplusplus
}
#endif

#endif //__CAPI_ADMINISTRATION_POLICY_H__
