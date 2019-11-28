/*
 *  Copyright (c) 2018-present Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file   eventfd.cpp
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Implementation of event notification.
 */

#include "eventfd.hpp"

#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <stdexcept>

namespace vist {
namespace event {

EventFD::EventFD(unsigned int initval, int flags)
	: fd(::eventfd(initval, flags))
{
	if (this->fd == -1)
		throw std::runtime_error("Failed to create eventfd.");
}

EventFD::~EventFD()
{
	::close(fd);
}

void EventFD::send(void)
{
	const std::uint64_t val = 1;
	if (::write(this->fd, &val, sizeof(val)) == -1)
		throw std::runtime_error("Failed to write to eventfd.");
}

void EventFD::receive(void)
{
	std::uint64_t val;
	if (::read(this->fd, &val, sizeof(val)) == -1)
		throw std::runtime_error("Failed to read from eventfd.");
}

int EventFD::getFd(void) const noexcept
{
	return this->fd;
}

} // namespace event
} // namespace vist
