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

#pragma once

#include <sys/eventfd.h>

namespace vist {
namespace rmi {
namespace impl {
namespace ondemand {

class EventFD final {
public:
	explicit EventFD(unsigned int initval = 0, int flags = EFD_SEMAPHORE | EFD_CLOEXEC);
	~EventFD();

	EventFD(const EventFD&) = delete;
	EventFD& operator=(const EventFD&) = delete;

	EventFD(EventFD&&) = delete;
	EventFD& operator=(EventFD&&) = delete;

	void send(void);
	void receive(void);

	int getFd(void) const noexcept;

private:
	int fd;
};

} // namespace ondemand
} // namespace impl
} // namespace rmi
} // namespace vist
