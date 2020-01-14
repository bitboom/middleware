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

#include <vist/rmi/impl/ondemand/eventfd.hpp>

#include <atomic>
#include <array>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <sys/epoll.h>

namespace vist {
namespace rmi {
namespace impl {
namespace ondemand {

class Mainloop {
public:
	using OnEvent = std::function<void(void)>;
	using OnError = std::function<void(void)>;
	using Stopper = std::function<bool(void)>;

	Mainloop();
	virtual ~Mainloop();

	Mainloop(const Mainloop&) = delete;
	Mainloop& operator=(const Mainloop&) = delete;

	Mainloop(Mainloop&&) = delete;
	Mainloop& operator=(Mainloop&&) = delete;

	void addHandler(const int fd, OnEvent&& onEvent, OnError&& = nullptr);
	void removeHandler(const int fd);

	/// Stopper is a predicate what returns a condition to stop mainloop
	/// when timeout is occured.
	void run(int timeout = -1, Stopper = nullptr);
	void stop(void);

private:
	// recursive_mutex makes additional calls to lock in calling thread.
	// And other threads will block (for calls to lock).
	// So, addHandler() can be called during dispatch().
	using Mutex = std::recursive_mutex;
	using Handler = std::pair<std::shared_ptr<OnEvent>, std::shared_ptr<OnError>>;
	using Listener = std::unordered_map<int, Handler>;

	Handler getHandler(const int fd);

	void prepare(void);

	void wait(int timeout, Stopper stopper);
	void dispatch(int size);

	Mutex mutex;
	Listener listener;
	EventFD wakeupSignal;

	int epollFd;
	std::atomic<bool> stopped;

	static constexpr int MAX_EVENTS = 16;
	std::array<::epoll_event, MAX_EVENTS> events;
};

} // namespace ondemand
} // namespace impl
} // namespace rmi
} // namespace vist
