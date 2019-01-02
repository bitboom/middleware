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

#include <sys/types.h>
#include <unistd.h>

#include <cstdint>

#include <klay/error.h>
#include <klay/eventfd.h>
#include <klay/exception.h>

namespace klay {

EventFD::EventFD(unsigned int initval, int flags)
{
	fd = ::eventfd(initval, flags);
	if (fd == -1) {
		throw klay::Exception(klay::GetSystemErrorMessage());
	}
}

EventFD::~EventFD()
{
	if (fd != -1) {
		::close(fd);
	}
}

void EventFD::close()
{
	::close(fd);
}

void EventFD::send()
{
	const std::uint64_t val = 1;
	if (::write(fd, &val, sizeof(val)) == -1) {
		throw klay::Exception(klay::GetSystemErrorMessage());
	}
}

void EventFD::receive()
{
	std::uint64_t val;
	if (::read(fd, &val, sizeof(val)) == -1) {
		throw klay::Exception(klay::GetSystemErrorMessage());
	}
}

} // namespace klay
