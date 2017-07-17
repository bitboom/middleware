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
/*
 * @file        exception.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Exception guard and custom exceptions
 */
#include "exception.hxx"

#include <klay/exception.h>

#include "logger.hxx"

namespace tanchor {

int exceptionGuard(const std::function<int()> &func)
{
	try {
		return func();
	} catch (const tanchor::Exception &e) {
		std::string errStr;
		switch (e.error()) {
		case ENOENT:
			errStr = "No such file or directory.";
			break;
		case ENOMEM:
			errStr = "Out of memory.";
			break;
		case EACCES:
		case EPERM:
			errStr = "Permission denied.";
			break;
		default:
			errStr = "Internal error.";
			break;
		}
		ERROR(SINK, errStr + e.what());
		return e.error();
	} catch (const runtime::Exception &e) {
		ERROR(SINK, e.what());
		return TRUST_ANCHOR_ERROR_INTERNAL;
	} catch (const std::invalid_argument &e) {
		ERROR(SINK, e.what());
		return TRUST_ANCHOR_ERROR_INVALID_PARAMETER;
	} catch (const std::logic_error &e) {
		ERROR(SINK, e.what());
		return TRUST_ANCHOR_ERROR_INTERNAL;
	} catch (const std::exception &e) {
		ERROR(SINK, e.what());
		return TRUST_ANCHOR_ERROR_INTERNAL;
	} catch (...) {
		ERROR(SINK, "Unknown exception occurred.");
		return TRUST_ANCHOR_ERROR_INTERNAL;
	}
}

Exception::Exception(int ec, const char *file, const char *function,
					 unsigned int line, const std::string &message) noexcept :
	m_ec(ec),
	m_message(FORMAT("[" << file << ":" << line << " " <<
					 function << "()]" << message))
{
	ERROR(SINK, this->m_message);
}

const char *Exception::what() const noexcept
{
	return this->m_message.c_str();
}

int Exception::error(void) const noexcept
{
	return this->m_ec;
}

} // namespace tanchor
