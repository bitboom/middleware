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

#include <cstring>

#include <klay/error.h>

namespace klay {

int Error::lastErrorCode()
{
	return errno;
}

std::string Error::message(int errorCode)
{
	char errmsg[256];
	return ::strerror_r(errorCode, errmsg, sizeof(errmsg));
}

std::string Error::message()
{
	return message(lastErrorCode());
}

std::string GetSystemErrorMessage(int errorCode)
{
	return Error::message(errorCode);
}

std::string GetSystemErrorMessage()
{
	return Error::message();
}

} // namespace klay
