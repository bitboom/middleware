/*
 *  Copyright (c) 2018 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "mainloop.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <cstring>

#include <unistd.h>
#include <errno.h>

namespace vist {
namespace rmi {
namespace impl {
namespace ondemand {

Mainloop::Mainloop() :
	epollFd(::epoll_create1(EPOLL_CLOEXEC)),
	stopped(false)
{
	if (epollFd == -1)
		THROW(ErrCode::RuntimeError) << "Failed to create epoll instance.";
}

Mainloop::~Mainloop()
{
	::close(this->epollFd);
}

void Mainloop::addHandler(const int fd, OnEvent&& onEvent, OnError&& onError)
{
	std::lock_guard<Mutex> lock(mutex);

	if (this->listener.find(fd) != this->listener.end())
		THROW(ErrCode::RuntimeError) << "Event is already registered.";

	::epoll_event event;
	std::memset(&event, 0, sizeof(epoll_event));

	event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
	event.data.fd = fd;

	if (::epoll_ctl(this->epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
		THROW(ErrCode::RuntimeError) << "Failed to add event source.";

	auto onEventPtr = std::make_shared<OnEvent>(onEvent);
	auto onErrorPtr = (onError != nullptr) ? std::make_shared<OnError>(onError) : nullptr;

	auto handler = std::make_pair(std::move(onEventPtr), std::move(onErrorPtr));

	this->listener.insert({fd, handler});
	DEBUG(VIST) << "FD[" << fd << "] listens to events.";
}

void Mainloop::removeHandler(const int fd)
{
	std::lock_guard<Mutex> lock(mutex);

	auto iter = this->listener.find(fd);
	if (iter == this->listener.end())
		return;

	this->listener.erase(iter);

	::epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
}

bool Mainloop::prepare(void)
{
	auto wakeup = [this]() {
		this->wakeupSignal.receive();
		this->removeHandler(this->wakeupSignal.getFd());
		this->stopped = true;
	};

	DEBUG(VIST) << "Add eventfd to mainloop for wakeup: " << this->wakeupSignal.getFd();
	this->addHandler(this->wakeupSignal.getFd(), wakeup);
}

bool Mainloop::dispatch(int timeout) noexcept
{
	int nfds;
	::epoll_event event[MAX_EPOLL_EVENTS];

	do {
		errno = 0;
		nfds = ::epoll_wait(epollFd, event, MAX_EPOLL_EVENTS, timeout);
	} while ((nfds == -1) && (errno == EINTR));

	if (nfds <= 0)
		return false;

	for (int i = 0; i < nfds; i++) {
		std::shared_ptr<OnEvent> onEvent;
		std::shared_ptr<OnError> onError;

		{
			std::lock_guard<Mutex> lock(mutex);

			auto iter = this->listener.find(event[i].data.fd);
			if (iter == this->listener.end())
				continue;

			onEvent = iter->second.first;
			onError = iter->second.second;
		}

		try {
			if ((event[i].events & (EPOLLHUP | EPOLLRDHUP))) {
				WARN(VIST) << "Connected client might be disconnected.";
				if (onError != nullptr)
					(*onError)();
			} else {
				(*onEvent)();
			}

		} catch (const std::exception& e) {
			ERROR(VIST) << e.what();
		}
	}

	return true;
}

void Mainloop::run(int timeout)
{
	bool done = false;
	this->stopped = false;

	this->prepare();

	while (!this->stopped && !done)
		done = !dispatch(timeout);
}

void Mainloop::stop(void)
{
	this->wakeupSignal.send();
}

} // namespace ondemand
} // namespace impl
} // namespace rmi
} // namespace vist
