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

#ifndef __RUNTIME_EXCEPTION_H__
#define __RUNTIME_EXCEPTION_H__

#include <stdexcept>
#include <string>

namespace runtime {

class Exception: public std::runtime_error {
public:
	Exception(const std::string& error) :
		std::runtime_error(error)
	{
	}

	virtual ~Exception() {}
	virtual const char *name() const;
	virtual const char *className() const;
};

#define EXCEPTION_DEFINE(CLS)             \
	class CLS : public Exception {        \
	public:                               \
		CLS(const std::string& error);    \
		~CLS() {}                         \
		const char *className() const;    \
		const char *name() const;         \
	};


#define EXCEPTION_IMPLEMENT(CLS, NAME)    \
	CLS::CLS(const std::string& error) :  \
		Exception(error)                  \
	{                                     \
	}                                     \
	const char *CLS::className() const    \
	{                                     \
		return STRINGIFY(CLS);            \
	}                                     \
	const char *CLS::name() const         \
	{                                     \
		return NAME;                      \
	}

EXCEPTION_DEFINE(AssertionViolationException)
EXCEPTION_DEFINE(NullPointerException)
EXCEPTION_DEFINE(InvalidArgumentException)
EXCEPTION_DEFINE(NotImplementedException)
EXCEPTION_DEFINE(RangeException)
EXCEPTION_DEFINE(NotFoundException)
EXCEPTION_DEFINE(UnsupportedException)
EXCEPTION_DEFINE(TimeoutException)
EXCEPTION_DEFINE(NoPermissionException)
EXCEPTION_DEFINE(OutOfMemoryException)
EXCEPTION_DEFINE(IOException)
} // namespace runtime
#endif //__RUNTIME_EXCEPTION_H__
