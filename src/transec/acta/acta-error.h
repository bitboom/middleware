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
 * @file acta_error.h
 * @brief Error enums and defines.
 */


#ifndef ACTA_ERROR_H
#define ACTA_ERROR_H


#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif


#define ACTA_ERROR_BASE -0x01000000


/**
 * @brief Enumeration for ACTA error values.
 */
typedef enum {
	ACTA_ERROR_NONE = 0,
	ACTA_ERROR_INVALID_PARAMETER = -EINVAL,
	ACTA_ERROR_OUT_OF_MEMORY = -ENOMEM,
	ACTA_ERROR_INTERNAL = ACTA_ERROR_BASE | 0x01
} acta_error_e;


#ifdef __cplusplus
} /* extern */
#endif


#endif /* ACTA_ERROR_H */
