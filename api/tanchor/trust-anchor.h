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
 * @file trust-anchor.h
 * @brief Trust anchor CAPI header
 */


#ifndef TRUST_ANCHOR_H
#define TRUST_ANCHOR_H


#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

#include <tanchor/error.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Install trust anchor.
 * @details Package custom certificates would be installed on system.
 *          This function should be called before trust_anchor_launch().
 * @param[in] package_id Package id
 * @param[in] uid user id
 * @param[in] pkg_certs_path Package custom certificates path
 * @param[in] with_system_certs Whether system certificates use or not
 * @return #TRUST_ANCHOR_ERROR_NONE on success,
 *         negative on error
 * @retval #TRUST_ANCHOR_ERROR_NONE Successful
 * @retval #TRUST_ANCHOR_ERROR_OUT_OF_MEMORY Out of memory error
 * @retval #TRUST_ANCHOR_ERROR_INVALID_PARAMETER Invalid parameter error
 * @retval #TRUST_ANCHOR_ERROR_PERMISSION_DENIED Permission denied error
 * @retval #TRUST_ANCHOR_ERROR_NO_SUCH_FILE No such file or directory error
 * @retval #TRUST_ANCHOR_ERROR_INTERNAL Internal error
 * @see trust_anchor_launch()
 * @see trust_anchor_uninstall()
 */
int trust_anchor_install(const char *package_id,
						 uid_t uid,
						 const char *pkg_certs_path,
						 bool with_system_certs);


/**
 * @brief Launch trust anchor.
 * @details After lauching trust anchor, package can use custom certificates.
 * @param[in] package_id Package id
 * @param[in] uid user id
 * @return #TRUST_ANCHOR_ERROR_NONE on success,
 *         negative on error
 * @retval #TRUST_ANCHOR_ERROR_NONE Successful
 * @retval #TRUST_ANCHOR_ERROR_OUT_OF_MEMORY Out of memory error
 * @retval #TRUST_ANCHOR_ERROR_INVALID_PARAMETER Invalid parameter error
 * @retval #TRUST_ANCHOR_ERROR_PERMISSION_DENIED Permission denied error
 * @retval #TRUST_ANCHOR_ERROR_NO_SUCH_FILE No such file or directory error
 * @retval #TRUST_ANCHOR_ERROR_INTERNAL Internal error
 * @retval #TRUST_ANCHOR_ERROR_NOT_INSTALLED Not installed error
 * @see trust_anchor_install()
 */
int trust_anchor_launch(const char *package_id, uid_t uid);


/**
 * @brief Uninstall trust anchor.
 * @param[in] package_id Package id
 * @param[in] uid user id
 * @return #TRUST_ANCHOR_ERROR_NONE on success,
 *         negative on error
 * @retval #TRUST_ANCHOR_ERROR_NONE Successful
 * @retval #TRUST_ANCHOR_ERROR_OUT_OF_MEMORY Out of memory error
 * @retval #TRUST_ANCHOR_ERROR_INTERNAL Internal error
 * @see trust_anchor_install()
 */
int trust_anchor_uninstall(const char *package_id, uid_t uid);


#ifdef __cplusplus
} /* extern */
#endif


#endif /* TRUST_ANCHOR_H */
