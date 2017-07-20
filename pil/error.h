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

#ifndef __DPM_ERROR_H__
#define __DPM_ERROR_H__

#include <tizen.h>
#include <tizen_type.h>

#define RET_ON_FAILURE(cond, ret) \
{                                 \
	if (!(cond))                  \
		return (ret);             \
}

typedef enum {
	DPM_ERROR_NONE                 = TIZEN_ERROR_NONE,                 /**< The operation was successful */
	DPM_ERROR_INVALID_PARAMETER    = TIZEN_ERROR_INVALID_PARAMETER,    /**< Invalid parameter */
	DPM_ERROR_CONNECTION_REFUSED   = TIZEN_ERROR_CONNECTION_REFUSED,   /**< Connection refused */
	DPM_ERROR_TIMED_OUT            = TIZEN_ERROR_TIMED_OUT,            /**< Time out */
	DPM_ERROR_PERMISSION_DENIED    = TIZEN_ERROR_PERMISSION_DENIED,    /**< Access privilege is not sufficient */
	DPM_ERROR_NOT_PERMITTED        = TIZEN_ERROR_NOT_PERMITTED,        /**< Operation not permitted */
	DPM_ERROR_FILE_EXISTS          = TIZEN_ERROR_FILE_EXISTS,          /**< File exists */
	DPM_ERROR_OUT_OF_MEMORY        = TIZEN_ERROR_OUT_OF_MEMORY,        /**< Out of memory */
	DPM_ERROR_NO_DATA              = TIZEN_ERROR_NO_DATA               /**< No Data */
} DevicePolicyError;

#endif //! __DPM_ERROR_H__
