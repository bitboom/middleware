/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @brief Enum based exception handling.
 * @usage
 *  boilerplate : THROW(${ERROR_CODE: ENUM_TYPE}) << ${MESSAGE_STREAM}
 *
 *  enum class ErrCode {
 *    LogicError = 1,
 *    RuntimeError
 *  };
 *
 *  try {
 *    THROW(ErrCode::LogicError) << "Additional information about exception";
 *  } catch (const vist::Exception<ErrCode>& e) {
 *    ErrCode ec = e.get();
 *    std::string message = e.what();
 *  }
 */

#pragma once

#include <cstring>
#include <exception>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>

#include <boost/core/demangle.hpp>

#ifndef __FILENAME__
#define __FILENAME__ \
	::strrchr(__FILE__, '/') ? ::strrchr(__FILE__, '/') + 1 : __FILE__
#endif

#define THROW(ec) \
	throw vist::Exception<decltype(ec)>(ec,__FILENAME__,__FUNCTION__,__LINE__)

namespace vist {

enum class ErrCode {
	LogicError = 0, /// Includes invalid_argument
	RuntimeError,
	BadCast
};

template <typename ErrCode>
class Exception final : public std::exception {
public:
	static_assert(std::is_enum<ErrCode>::value, "Error code must be enum type.");

	using Self = Exception<ErrCode>;

	Exception(ErrCode ec, const char *file,
			  const char *function, unsigned int line) noexcept : ec(ec)
	{
		std::stringstream ss;
		ss << "[" << file << ":" << line << " " << function << "()]"
		   << "[" << boost::core::demangle(typeid(ec).name()) << "("
		   << static_cast<typename std::underlying_type_t<ErrCode>>(ec) << ")] ";
		message = ss.str();
	}

	virtual const char* what() const noexcept override
	{
		return message.c_str();
	}

	inline ErrCode get() const noexcept
	{
		return ec;
	}

	template<typename T>
	Self& operator<<(const T& arg) noexcept
	{
		std::stringstream ss;
		ss << arg;
		message += ss.str();
		return *this;
	}

private:
	ErrCode ec;
	std::string message;
};

} // namespace vist
