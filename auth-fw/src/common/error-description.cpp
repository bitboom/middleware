/*
 *  Copyright (c) 2000 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 *
 * @file        error-description.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       Implementatin of errorToString function.
 */
#include "error-description.h"

#include <string.h>
#include <symbol-visibility.h>

#define MAX_BUF 256

namespace AuthPasswd {

COMMON_API
std::string errnoToString(int err)
{
	char buffer[MAX_BUF] = {};
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE

	if (0 == strerror_r(err, buffer, MAX_BUF))
		return std::string(buffer);

#else
	char *result = strerror_r(err, buffer, MAX_BUF);

	if (result)
		return std::string(result);

#endif
	return std::string();
}

} // namespace AuthPasswd
