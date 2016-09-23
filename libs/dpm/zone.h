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

#ifndef __CAPI_ZONE_POLICY_H__
#define __CAPI_ZONE_POLICY_H__

#include <dpm/device-policy-manager.h>
#include <dpm/context.h>

/**
 * @file zone.h
 * @brief This file provides APIs to control zones
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  CAPI_DPM_ZONE_POLICY_MODULE
 * @{
 */

/**
 * @partner
 * @brief       Creates a new zone.
 * @details     An administrator can use this API to create a container. Once the container
 *              is created, the admin package given to the parameter will be installed inside
 *              the container. Then ownership of the container gets transferred to the admin
 *              package from the client which triggered the container creation process.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.zone
 * @param[in]   handle Device policy manager handle
 * @param[in]   name The zone name to be created
 * @param[in]   pkgname Admin package container ownership will be transferred
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_NOT_PERMITTED Operation not permitted
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_zone_destroy()
 * @see         dpm_zone_create_iterator()
 */
int dpm_zone_create(device_policy_manager_h handle, const char* name, const char* pkgname);

/**
 * @partner
 * @brief       Removes existing zone.
 * @details     Administrator can use this API to remove zone. All file system objects
 *              created for the zone will be also erased.
 * @since_tizen 3.0
 * @privlevel	partner
 * @privilege	%http://tizen.org/privilege/dpm.zone
 * @param[in]   handle Device policy manager handle
 * @param[in]   name The zone name to be removed
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API or the caller is not the owner
 *              of the zone
 * @pre         The handle must be created by dpm_manager_create().
 * @pre         The zone corresponding to the given name must be
 *              created before use of this API.
 * @see         dpm_manager_create()
 * @see         dpm_zone_create()
 * @see         dpm_zone_create_iterator()
 */
int dpm_zone_destroy(device_policy_manager_h handle, const char* name);

/**
 * @brief       Enumeration for zone state
 * @since_tizen 3.0
 */
typedef enum {
	DPM_ZONE_STATE_LOCKED       = 0x01,  /**< Zone has been defined, but it can not start. */
	DPM_ZONE_STATE_RUNNING      = 0x02, /**< Zone has been started. */
	DPM_ZONE_STATE_ALL          = 0xff  /**< This presents all of the state  */
} dpm_zone_state_e;

/**
 * @brief       Gets the zone state.
 * @details     This API can be used to get the state of the zone. The zone can
 *              have one of the three states(running, locked).
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[in]   name The zone name
 * @param[out]  state The zone state
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_NO_DATA No such zone to get state
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_zone_create()
 * @see         dpm_zone_destroy()
 */
int dpm_zone_get_state(device_policy_manager_h handle, const char* name, dpm_zone_state_e *state);

/**
 * @brief       Called to get all the name of created zones.
 * @since_tizen 3.0
 * @param[in]   name The zone name
 * @param[in]   user_data The user data passed from dpm_zone_foreach_name
 * @return      true to continue with the next iteration of the loop, otherwise false to break out out the loop
 * @see         dpm_zone_foreach_name()
 */
typedef bool(*dpm_zone_foreach_name_cb)(const char* name, void *user_data);

/**
 * @brief       Retrieves all the name of created zones
 * @details     This API calls dpm_zone_foreach_cb() once for each zone name
 *              with traversing the created zones list.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[in]   state a combination of the zone state to look
 * @param[in]   callback The iteration callback function
 * @param[in]   user_data The user data passed to the callback function
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_zone_create()
 * @see         dpm_zone_destroy()
 */
int dpm_zone_foreach_name(device_policy_manager_h handle,
						  dpm_zone_state_e state,
						  dpm_zone_foreach_name_cb callback, void* user_data);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CAPI_ZONE_POLICY__ */
