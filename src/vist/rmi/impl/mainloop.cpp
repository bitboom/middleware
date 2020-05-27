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

#include "mainloop.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <cstring>

#include <unistd.h>
#include <errno.h>

namespace vist {
namespace rmi {
namespace impl {

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

	if (this->listener.find(fd) != this->listener.end()) {
		WARN(VIST) << "Event is already registered.";
		return;
	}

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
		THROW(ErrCode::RuntimeError) << "Not found file descriptor.";

	this->listener.erase(iter);

	::epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
}

Mainloop::Handler Mainloop::getHandler(const int fd)
{
	std::lock_guard<Mutex> lock(mutex);

	auto iter = this->listener.find(fd);
	if (iter == this->listener.end())
		THROW(ErrCode::RuntimeError) << "Not found file descriptor.";

	return std::make_pair(iter->second.first, iter->second.second);
}

void Mainloop::prepare(void)
{
	auto wakeup = [this]() {
		this->wakeupSignal.receive();
		this->removeHandler(this->wakeupSignal.getFd());
		this->stopped = true;
	};

	DEBUG(VIST) << "Add eventfd to mainloop for wakeup: " << this->wakeupSignal.getFd();
	this->addHandler(this->wakeupSignal.getFd(), wakeup);
}

void Mainloop::wait(int timeout, Stopper stopper)
{
	int nfds = 0;
	do {
		errno = 0;
		nfds = ::epoll_wait(this->epollFd, this->events.data(), MAX_EVENTS, timeout);
		if (errno == EINTR)
			WARN(VIST) << "The call was interrupted by a signal handler.";
	} while ((nfds == -1) && (errno == EINTR));

	if (nfds == 0) {
		DEBUG(VIST) << "Mainloop is stopped by timeout.";

		if (stopper())
			this->stopped = true;

		return;
	}

	if (nfds < 0)
		THROW(ErrCode::RuntimeError) << "Failed to wait epoll events: " << errno;

	this->dispatch(nfds);
}

void Mainloop::dispatch(int size)
{
	for (int i = 0; i < size; i++) {
		auto handler = this->getHandler(this->events[i].data.fd);
		auto onEvent = handler.first;
		auto onError = handler.second;

		try {
			if ((this->events[i].events & (EPOLLHUP | EPOLLRDHUP))) {
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
}

void Mainloop::run(int timeout, Stopper stopper)
{
	this->stopped = false;
	this->prepare();

	if (stopper == nullptr)
		stopper = []() -> bool { return true; };

	while (!this->stopped)
		this->wait(timeout, stopper);
}

void Mainloop::stop(void)
{
	this->wakeupSignal.send();
}

} // namespace impl
} // namespace rmi
} // namespace vist
