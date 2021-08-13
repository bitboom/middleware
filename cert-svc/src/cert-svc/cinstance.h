/**
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        cinstance.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       This is part of C api for ValidationCore.
 */
#ifndef _CERTSVC_CINSTANCE_H_
#define _CERTSVC_CINSTANCE_H_

#include <cert-svc/cerror.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CertSvcInstance_t {
	void *privatePtr;
} CertSvcInstance;

/**
 * @brief Initializes and returns a CertSvcInstance.
 *
 * @remarks @a instance should be released using certsvc_instance_free().
 *
 * @param[out] instance A pointer of CertSvcInstance.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see certsvc_instance_free()
 */
int certsvc_instance_new(CertSvcInstance *instance);

/**
 * @brief Releases and reallocates a CertSvcInstance.
 *
 * @remarks @a instance will be released and reallocated.
 * @remarks strings allocated by certsvc_certificate_get_string_field() will be
 *          released also.
 * @remarks @a instance should be released using certsvc_instance_free().
 *
 * @param[in] instance CertSvcInstance returned by certsvc_instance_new().
 *
 * @see certsvc_certificate_get_string_field()
 * @see certsvc_instance_free()
 */
void certsvc_instance_reset(CertSvcInstance instance);

/**
 * @brief Releases a CertSvcInstance.
 *
 * @remarks @a instance will be released.
 * @remarks strings allocated by certsvc_certificate_get_string_field() will be
 *          released also.
 * @remarks It is safe use this function after use certsvc_string_free().
 *
 * @param[in] instance CertSvcInstance returned by certsvc_instance_new().
 *
 * @see certsvc_instance_create()
 * @see certsvc_certificate_get_string_field()
 * @see certsvc_string_free()
 */
void certsvc_instance_free(CertSvcInstance instance);

#ifdef __cplusplus
}
#endif

#endif // _CERTSVC_CINSTANCE_H_

