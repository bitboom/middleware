/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */


/**
 * @file trust-anchor.h
 * @brief Trust anchor CAPI header
 */


#ifndef TRUST_ANCHOR_H
#define TRUST_ANCHOR_H


#include <stddef.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Install trust anchor for global app.
 * @details App custom certificates would be installed on system.
 *          This function should be called before trust_anchor_global_launch().
 * @param[in] package_id Global package id
 * @param[in] app_certificates_path App custom certificates path
 * @param[in] with_system_certificates Whether system certificates use or not
 * @return #TRUST_ANCHOR_ERROR_NONE on success,
 *         negative on error
 * @retval #TRUST_ANCHOR_ERROR_NONE Successful
 * @retval #TRUST_ANCHOR_ERROR_OUT_OF_MEMORY Out of memory error
 * @retval #TRUST_ANCHOR_ERROR_INTERNAL Internal error
 * @see trust_anchor_global_launch()
 * @see trust_anchor_global_uninstall()
 */
int trust_anchor_global_install(const char *package_id,
								const char *app_certificates_path,
								bool with_system_certificates);


/**
 * @brief Install trust anchor for user app.
 * @details App custom certificates would be installed on system.
 *          This function should be called before trust_anchor_usr_launch().
 * @param[in] package_id User package id
 * @param[in] app_certificates_path App custom certificates path
 * @param[in] uid user id
 * @param[in] with_system_certificates Whether system certificates use or not
 * @return #TRUST_ANCHOR_ERROR_NONE on success,
 *         negative on error
 * @retval #TRUST_ANCHOR_ERROR_NONE Successful
 * @retval #TRUST_ANCHOR_ERROR_OUT_OF_MEMORY Out of memory error
 * @retval #TRUST_ANCHOR_ERROR_INTERNAL Internal error
 * @see trust_anchor_usr_launch()
 * @see trust_anchor_usr_uninstall()
 */
int trust_anchor_usr_install(const char *package_id,
							 const char *app_certificates_path,
							 uid_t uid,
							 bool with_system_certificates);


/**
 * @brief Launch trust anchor for global app.
 * @details After lauching trust anchor, app can use custom certificates.
 * @param[in] package_id Global package id
 * @param[in] app_certificates_path App custom certificates path
 * @param[in] with_system_certificates Whether system certificates use or not
 * @return #TRUST_ANCHOR_ERROR_NONE on success,
 *         negative on error
 * @retval #TRUST_ANCHOR_ERROR_NONE Successful
 * @retval #TRUST_ANCHOR_ERROR_OUT_OF_MEMORY Out of memory error
 * @retval #TRUST_ANCHOR_ERROR_INTERNAL Internal error
 * @see trust_anchor_global_install()
 */
int trust_anchor_global_launch(const char *package_id,
							   const char *app_certificates_path,
							   bool with_system_certificates);


/**
 * @brief Launch trust anchor for user app.
 * @details After lauching trust anchor, app can use custom certificates.
 * @param[in] package_id User package id
 * @param[in] app_certificates_path App custom certificates path
 * @param[in] with_system_certificates Whether system certificates use or not
 * @return #TRUST_ANCHOR_ERROR_NONE on success,
 *         negative on error
 * @retval #TRUST_ANCHOR_ERROR_NONE Successful
 * @retval #TRUST_ANCHOR_ERROR_OUT_OF_MEMORY Out of memory error
 * @retval #TRUST_ANCHOR_ERROR_INTERNAL Internal error
 * @see trust_anchor_global_install()
 */
int trust_anchor_usr_launch(const char *package_id,
							const char *app_certificates_path,
							uid_t uid,
							bool with_system_certificates);


/**
 * @brief Uninstall trust anchor for global app.
 * @param[in] package_id Global package id
 * @param[in] app_certificates_path App custom certificates path
 * @return #TRUST_ANCHOR_ERROR_NONE on success,
 *         negative on error
 * @retval #TRUST_ANCHOR_ERROR_NONE Successful
 * @retval #TRUST_ANCHOR_ERROR_OUT_OF_MEMORY Out of memory error
 * @retval #TRUST_ANCHOR_ERROR_INTERNAL Internal error
 * @see trust_anchor_global_install()
 */
int trust_anchor_global_uninstall(const char *package_id,
								  const char *app_certificates_path);


/**
 * @brief Uninstall trust anchor for user app.
 * @param[in] package_id User package id
 * @param[in] app_certificates_path App custom certificates path
 * @return #TRUST_ANCHOR_ERROR_NONE on success,
 *         negative on error
 * @retval #TRUST_ANCHOR_ERROR_NONE Successful
 * @retval #TRUST_ANCHOR_ERROR_OUT_OF_MEMORY Out of memory error
 * @retval #TRUST_ANCHOR_ERROR_INTERNAL Internal error
 * @see trust_anchor_usr_install()
 */
int trust_anchor_usr_uninstall(const char *package_id,
							   const char *app_certificates_path,
							   uid_t uid);


#ifdef __cplusplus
} /* extern */
#endif


#endif /* TRUST_ANCHOR_H */
