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

#include "socket.hpp"

#include <vist/logger.hpp>

#include <fstream>
#include <iostream>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

namespace vist {
namespace rmi {
namespace impl {
namespace ondemand {

namespace {

void set_cloexec(int fd)
{
	if (::fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
		THROW(ErrCode::RuntimeError) << "Failed to set CLOSEXEC.";
}

} // anonymous namespace

Socket::Socket(int fd) noexcept : fd(fd)
{
}

Socket::Socket(const std::string& path)
{
	if (path.size() >= sizeof(::sockaddr_un::sun_path))
		THROW(ErrCode::LogicError) << "Socket path size is wrong.";

	int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1)
		THROW(ErrCode::RuntimeError) << "Failed to create socket.";

	set_cloexec(fd);

	::sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	::strncpy(addr.sun_path, path.c_str(), sizeof(sockaddr_un::sun_path) - 1);
	addr.sun_path[sizeof(sockaddr_un::sun_path) - 1] = '\0';

	if (addr.sun_path[0] == '@')
		addr.sun_path[0] = '\0';

	struct stat buf;
	if (::stat(path.c_str(), &buf) == 0)
		if (::unlink(path.c_str()) == -1)
			THROW(ErrCode::RuntimeError) << "Failed to remove exist socket.";

	if (::bind(fd, reinterpret_cast<::sockaddr*>(&addr), sizeof(::sockaddr_un)) == -1) {
		::close(fd);
		THROW(ErrCode::RuntimeError) << "Failed to bind.";
	}

	if (::listen(fd, MAX_BACKLOG_SIZE) == -1) {
		::close(fd);
		THROW(ErrCode::RuntimeError) << "Failed to liten.";
	}

	this->fd = fd;

	DEBUG(VIST) << "Socket is created: " << path << ", and is listening.. fd[" << fd << "]";
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
	errno = 0;
	int fd = ::accept(this->fd, nullptr, nullptr);
	if (fd == -1)
		THROW(ErrCode::RuntimeError) << "Failed to accept: " << errno;

	set_cloexec(fd);

	return Socket(fd);
}

Socket Socket::connect(const std::string& path)
{
	if (path.size() >= sizeof(::sockaddr_un::sun_path))
		THROW(ErrCode::LogicError) << "Socket path size is wrong.";

	int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1)
		THROW(ErrCode::RuntimeError) << "Failed to create socket.";

	set_cloexec(fd);

	::sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	::strncpy(addr.sun_path, path.c_str(), sizeof(::sockaddr_un::sun_path));

	if (addr.sun_path[0] == '@')
		addr.sun_path[0] = '\0';

	DEBUG(VIST) << "Start to connect: " << path;
	errno = 0;
	if (::connect(fd, reinterpret_cast<::sockaddr*>(&addr), sizeof(sockaddr_un)) == -1) {
		::close(fd);
		ERROR(VIST) << "Failed to connect to: " << path;
		THROW(ErrCode::RuntimeError) << "Failed to connect to: " << path
									 << ", with: " << errno;
	}

	return Socket(fd);
}

int Socket::getFd(void) const noexcept
{
	return this->fd;
}

} // namespace ondemand
} // namespace impl
} // namespace rmi
} // namespace vist
