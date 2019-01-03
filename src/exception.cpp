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
#include <klay/exception.h>
#include <klay/preprocessor.h>

namespace klay {

const char *Exception::name() const
{
	return "Exception";
}

const char *Exception::className() const
{
	return name();
}

EXCEPTION_IMPLEMENT(AssertionViolationException, "Assertion violation")
EXCEPTION_IMPLEMENT(NullPointerException, "Null pointer")
EXCEPTION_IMPLEMENT(InvalidArgumentException, "Invalid argument")
EXCEPTION_IMPLEMENT(NotImplementedException, "Not implemented")
EXCEPTION_IMPLEMENT(RangeException, "Out of range")
EXCEPTION_IMPLEMENT(NotFoundException, "Not found")
EXCEPTION_IMPLEMENT(UnsupportedException, "Unsupported")
EXCEPTION_IMPLEMENT(TimeoutException, "Timeout")
EXCEPTION_IMPLEMENT(NoPermissionException, "No permission")
EXCEPTION_IMPLEMENT(OutOfMemoryException, "Out of memory")
EXCEPTION_IMPLEMENT(IOException, "I/O error")
} // namespace klay
