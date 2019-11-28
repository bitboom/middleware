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
 * @file   mainloop.hpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  The loop for events.
 */

#pragma once

#include <vist/event/eventfd.hpp>

#include <sys/epoll.h>

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace vist {
namespace event {

class Mainloop {
public:
	using OnEvent = std::function<void(void)>;
	using OnError = std::function<void(void)>;

	Mainloop();
	virtual ~Mainloop();

	Mainloop(const Mainloop&) = delete;
	Mainloop& operator=(const Mainloop&) = delete;

	Mainloop(Mainloop&&) = delete;
	Mainloop& operator=(Mainloop&&) = delete;

	void addHandler(const int fd, OnEvent&& onEvent, OnError&& = nullptr);
	void removeHandler(const int fd);

	void run(int timeout = -1);
	void stop(void);

private:
	// recursive_mutex makes additional calls to lock in calling thread.
	// And other threads will block (for calls to lock).
	// So, addHandler() can be called during dispatch().
	using Mutex = std::recursive_mutex;
	using Handler = std::pair<std::shared_ptr<OnEvent>, std::shared_ptr<OnError>>;
	using Listener = std::unordered_map<int, Handler>;

	bool prepare(void);

	bool dispatch(const int timeout) noexcept;

	Mutex mutex;
	Listener listener;
	EventFD wakeupSignal;

	int epollFd;
	std::atomic<bool> stopped;

	const int MAX_EPOLL_EVENTS = 16;
};

} // namespace event
} // namespace vist
