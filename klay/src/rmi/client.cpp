/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <klay/exception.h>
#include <klay/rmi/client.h>

namespace klay {
namespace rmi {

void DefaultExceptionModel::raise(const std::string& target, const std::string& msg)
{
	if (target == "InvalidArgumentException")
		throw klay::InvalidArgumentException(msg);
	else if (target == "NotImplementedException")
		throw klay::InvalidArgumentException(msg);
	else if (target == "RangeException")
		throw klay::RangeException(msg);
	else if (target == "NotFoundException")
		throw klay::NotFoundException(msg);
	else if (target == "UnsupportedException")
		throw klay::UnsupportedException(msg);
	else if (target == "NoPermissionException")
		throw klay::NoPermissionException(msg);
	else if (target == "IOException")
		throw klay::IOException(msg);
	else if (target == "OutOfMemoryException")
		throw klay::OutOfMemoryException(msg);
	else
		throw klay::Exception(msg);
}

} // namespace rmi
} // namespace klay
