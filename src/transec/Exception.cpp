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
/*
 * @file        Exception.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Exception guard and custom exceptions
 */
#include "Exception.h"

#include <exception>

#include <klay/exception.h>
#include <klay/audit/logger.h>

namespace transec {

int exceptionGuard(const std::function<int()> &func)
{
	// TODO add custom error code
	try {
		return func();
	} catch (runtime::Exception &e) {
		ERROR(e.what());
		return -1;
	} catch (const std::invalid_argument &e) {
		ERROR("Invalid argument: " << e.what());
		return -1;
	} catch (const std::exception &e) {
		ERROR(e.what());
		return -1;
	} catch (...) {
		ERROR("Unknown exception occurred.");
		return -1;
	}
}

} // namespace transec
