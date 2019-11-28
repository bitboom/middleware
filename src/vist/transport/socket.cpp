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
 * @file   socket.cpp
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Implementation of Unix Domain Socket.
 */

#include "socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#include <fstream>
#include <iostream>

namespace vist {
namespace transport {

namespace {

void set_cloexec(int fd)
{
	if (::fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
		throw std::runtime_error("Failed to set CLOSEXEC.");
}

} // anonymous namespace

Socket::Socket(int fd) noexcept : fd(fd)
{
}

Socket::Socket(const std::string& path)
{
	if (path.size() >= sizeof(::sockaddr_un::sun_path))
		throw std::invalid_argument("Socket path size is wrong.");

	int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1)
		throw std::runtime_error("Failed to create socket.");

	set_cloexec(fd);

	::sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	::strncpy(addr.sun_path, path.c_str(), sizeof(sockaddr_un::sun_path));

	if (addr.sun_path[0] == '@')
		addr.sun_path[0] = '\0';

	struct stat buf;
	if (::stat(path.c_str(), &buf) == 0)
		if (::unlink(path.c_str()) == -1)
			throw std::runtime_error("Failed to remove exist socket.");

	if (::bind(fd, reinterpret_cast<::sockaddr*>(&addr), sizeof(::sockaddr_un)) == -1) {
		::close(fd);
		throw std::runtime_error("Failed to bind.");
	}

	if (::listen(fd, MAX_BACKLOG_SIZE) == -1) {
		::close(fd);
		throw std::runtime_error("Failed to liten.");
	}

	this->fd = fd;
}

Socket::Socket(Socket&& that) : fd(that.fd)
{
	that.fd = -1;
}

Socket& Socket::operator=(Socket&& that)
{
	if (this == &that)
		return *this;

	this->fd = that.fd;
	that.fd = -1;

	return *this;
}

Socket::~Socket(void)
{
	if (fd != -1)
		::close(fd);
}

Socket Socket::accept(void) const
{
	int fd = ::accept(this->fd, nullptr, nullptr);
	if (fd == -1)
		throw std::runtime_error("Failed to accept.");

	set_cloexec(fd);

	return Socket(fd);
}

Socket Socket::connect(const std::string& path)
{
	if (path.size() >= sizeof(::sockaddr_un::sun_path))
		throw std::invalid_argument("Socket path size is wrong.");

	int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1)
		throw std::runtime_error("Failed to create socket.");

	set_cloexec(fd);

	::sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	::strncpy(addr.sun_path, path.c_str(), sizeof(::sockaddr_un::sun_path));

	if (addr.sun_path[0] == '@')
		addr.sun_path[0] = '\0';

	if (::connect(fd, reinterpret_cast<::sockaddr*>(&addr), sizeof(sockaddr_un)) == -1) {
		::close(fd);
		throw std::runtime_error("Failed to connect.");
	}

	return Socket(fd);
}

int Socket::getFd(void) const noexcept
{
	return this->fd;
}

} // namespace transport
} // namespace vist
