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
 * @file        error-description.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       Implementatin of errorToString function.
 */
#ifndef _AUTH_PASSWD_ERROR_DESCRIPTION_
#define _AUTH_PASSWD_ERROR_DESCRIPTION_

#include <string>
#include <cerrno>

namespace AuthPasswd {

std::string errnoToString(int err = errno);

} // namespace AuthPasswd

#endif // _AUTH_PASSWD_ERROR_DESCRIPTION_
