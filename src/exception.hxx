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
 * @file        exception.h
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Exception guard and custom exceptions
 */
#pragma once

#include "tanchor/error.h"

#include <functional>
#include <string>
#include <exception>

#include "logger.hxx"

#define EXCEPTION_GUARD_START return tanchor::exceptionGuard([&]() {
#define EXCEPTION_GUARD_END   });

namespace tanchor {

int exceptionGuard(const std::function<int()> &);

class Exception : public std::exception {
public:
	Exception(int ec, const char *file, const char *function,
			  unsigned int line, const std::string &message) noexcept;
	virtual ~Exception() = default;
	virtual const char* what() const noexcept override;

	int error(void) const noexcept;

protected:
	int m_ec;
	std::string m_message;
};

} // namespace tanchor

#define __TANCHOR_THROW(ec, MESSAGE)                     \
	throw tanchor::Exception(ec, __FILE__, __FUNCTION__, \
							 __LINE__, FORMAT(MESSAGE))

#define ThrowExc(ec, MESSAGE) __TANCHOR_THROW(ec, MESSAGE)

#define ThrowErrno(ec, MESSAGE)                                      \
	do {                                                             \
		switch (ec) {                                                \
		case ENOENT:                                                 \
			ThrowExc(TRUST_ANCHOR_ERROR_NO_SUCH_FILE, MESSAGE);      \
		case ENOMEM:                                                 \
			ThrowExc(TRUST_ANCHOR_ERROR_OUT_OF_MEMORY, MESSAGE);     \
		case EACCES:                                                 \
		case EPERM:                                                  \
			ThrowExc(TRUST_ANCHOR_ERROR_PERMISSION_DENIED, MESSAGE); \
		default:                                                     \
			ThrowExc(TRUST_ANCHOR_ERROR_INTERNAL, MESSAGE);          \
		}                                                            \
	} while (0)
