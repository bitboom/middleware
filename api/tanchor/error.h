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
 * @file error.h
 * @brief Error enums and defines.
 */


#ifndef TRUST_ANCHOR_ERROR_H
#define TRUST_ANCHOR_ERROR_H


#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif


#define TRUST_ANCHOR_ERROR_BASE -0x01000000


/**
 * @brief Enumeration for trust anchor error values.
 */
typedef enum {
	TRUST_ANCHOR_ERROR_NONE = 0,
	TRUST_ANCHOR_ERROR_INVALID_PARAMETER = -EINVAL,
	TRUST_ANCHOR_ERROR_OUT_OF_MEMORY = -ENOMEM,
	TRUST_ANCHOR_ERROR_INTERNAL = TRUST_ANCHOR_ERROR_BASE | 0x01
} trust_anchor_error_e;


#ifdef __cplusplus
} /* extern */
#endif


#endif /* TRUST_ANCHOR_ERROR_H */
