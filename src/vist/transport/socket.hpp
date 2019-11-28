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
 * @file   socket.hpp
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Define Unix Domain Socket.
 */

#pragma once

#include <vist/exception.hpp>

#include <errno.h>
#include <unistd.h>

#include <cstddef>
#include <string>

namespace vist {
namespace transport {

class Socket {
public:
	explicit Socket(int fd) noexcept;
	explicit Socket(const std::string& path);
	virtual ~Socket(void);

	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	Socket(Socket&&);
	Socket& operator=(Socket&&);

	Socket accept(void) const;
	static Socket connect(const std::string& path);

	template<typename T>
	void send(const T* buffer, const std::size_t size = sizeof(T)) const;

	template<typename T>
	void recv(T* buffer, const std::size_t size = sizeof(T)) const;

	int getFd(void) const noexcept;

private:
	const int MAX_BACKLOG_SIZE = 100;

	int fd = -1;
};

template<typename T>
void Socket::send(const T *buffer, const std::size_t size) const
{
	std::size_t written = 0;
	while (written < size) {
		auto rest = reinterpret_cast<const unsigned char*>(buffer) + written;
		auto bytes = ::write(this->fd, rest, size - written);
		if (bytes >= 0)
			written += bytes;
		else if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			continue;
		else
			THROW(ErrCode::RuntimeError) << "Failed to write to socket.";
	}
}

template<typename T>
void Socket::recv(T *buffer, const std::size_t size) const
{
	std::size_t readen = 0;
	while (readen < size) {
		auto rest = reinterpret_cast<unsigned char*>(buffer) + readen;
		auto bytes = ::read(this->fd, rest, size - readen);
		if (bytes >= 0)
			readen += bytes;
		else if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			continue;
		else
			std::runtime_error("Failed to read.");
	}
}

} // namespace transport
} // namespace vist
