/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

/**
 *  @file smack.h
 *  @brief Smack agent module external C API.
 *  @author Dmytro Logachev (d.logachev@samsung.com)
 *  @date Created Oct 13, 2016
 *  @par In Samsung Ukraine R&D Center (SRK) under a contract between
 *  @par LLC "Samsung Electronics Ukraine Company" (Kyiv, Ukraine)
 *  @par and "Samsung Electronics Co", Ltd (Seoul, Republic of Korea)
 *  @par Copyright: (c) Samsung Electronics Co, Ltd 2016. All rights reserved.
**/

#ifndef __CAPI_SMACK_SECURITY_MONITOR_H__
#define __CAPI_SMACK_SECURITY_MONITOR_H__

#include <sys/types.h>

#include <dpm/device-policy-manager.h>

/**
 * @file smack.h
 * @brief This file provides APIs to get/release the smack report and issue count
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  CAPI_DSM_SMACK_SECURITY_MONITOR_MODULE
 * @{
 */

/**
 * @brief       Returns SMACK report.
 * @details     This API can be used for requesting current SMACK report
 * @remarks     The SMACK report output should be freed using dpm_smart_security_release_report()
 * @param[in]   handle SMACK report handle
 * @param[out]  report SMACK report string
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_OUT_OF_MEMORY Out of memory
 * @pre         handle must be created by dpm_context_create()
 */
int dpm_smack_get_report(device_policy_manager_h handle, char **report);

/**
 * @brief       Releases SMACK report.
 * @details     This API can be used for releasing previously got SMACK report
 * @remarks     The SMACK report should be got using tsm_smack_get_report()
 * @param[in]   report SMACK report output
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_OUT_OF_MEMORY Out of memory
 * @pre         The handle must be created by dpm_context_create().
 */
int dpm_smack_release_report(char **report);

/**
 * @brief       Returns number of SMACK issues.
 * @details     This API can be used for requesting current number of
                SMACK issues
 * @param[in]   handle SMACK report handle
 * @param[out]  count Number of SMACK Issues
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_OUT_OF_MEMORY Out of memory
 * @pre         The handle must be created by dpm_context_create().
 */
int dpm_smack_get_issue_count(device_policy_manager_h handle, int *count);

#ifdef __cplusplus
}
#endif

#endif //__CAPI_SMACK_SECURITY_MONITOR_H__
