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

#ifndef __RUNTIME_EVENTFD_H__
#define __RUNTIME_EVENTFD_H__

#include <sys/eventfd.h>

#include <klay/klay.h>

namespace klay {

class KLAY_EXPORT EventFD {
public:
	EventFD(unsigned int initval = 0, int flags = EFD_SEMAPHORE | EFD_CLOEXEC);
	~EventFD();

	EventFD(const EventFD&) = delete;
	EventFD& operator=(const EventFD&) = delete;

	void send();
	void receive();
	void close();

	int getFd() const
	{
		return fd;
	}

private:
	int fd;
};

} // namespace klay

namespace runtime = klay;

#endif //__RUNTIME_EVENTFD_H__
