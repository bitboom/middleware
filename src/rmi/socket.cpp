/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <systemd/sd-daemon.h>

#include <iostream>

#include <klay/error.h>
#include <klay/rmi/socket.h>

namespace klay {
namespace rmi {

namespace {

const int MAX_BACKLOG_SIZE = 100;

void setCloseOnExec(int fd)
{
	if (::fcntl(fd, F_SETFD, FD_CLOEXEC) == -1) {
		throw SocketException(runtime::GetSystemErrorMessage());
	}
}

Credentials getCredentials(int fd)
{
	socklen_t length = 256;
	char buf[256];
	struct ucred cred;
	socklen_t credsz = sizeof(cred);
	if (::getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &cred, &credsz)) {
		throw SocketException(runtime::GetSystemErrorMessage());
	}

	if (::getsockopt(fd, SOL_SOCKET, SO_PEERSEC, buf, &length)) {
		throw SocketException(runtime::GetSystemErrorMessage());
	}

	return {cred.pid, cred.uid, cred.gid, buf};
}

} // namespace

Socket::Socket(int fd, bool autoclose) :
	socketFd(fd), autoClose(autoclose)
{
}

Socket::Socket(Socket&& socket) noexcept :
	socketFd(socket.socketFd),
	autoClose(socket.autoClose)
{
	socket.socketFd = -1;
}

Socket::~Socket() noexcept
{
	if ((socketFd != -1) && (autoClose)) {
		::close(socketFd);
	}
}

Socket Socket::accept()
{
	int sockfd = ::accept(socketFd, nullptr, nullptr);
	if (sockfd == -1) {
		throw SocketException(runtime::GetSystemErrorMessage());
	}

	setCloseOnExec(sockfd);

	return Socket(sockfd);
}

int Socket::getFd() const
{
	return socketFd;
}

Credentials Socket::getPeerCredentials() const
{
	return getCredentials(socketFd);
}

void Socket::read(void *buffer, const size_t size) const
{
	size_t total = 0;

	while (total < size) {
		int bytes = ::read(socketFd, reinterpret_cast<char*>(buffer) + total, size - total);
		if (bytes >= 0) {
			total += bytes;
		} else if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
			continue;
		} else {
			throw SocketException(runtime::GetSystemErrorMessage());
		}
	}
}

void Socket::write(const void *buffer, const size_t size) const
{
	size_t written = 0;

	while (written < size) {
		int bytes = ::write(socketFd, reinterpret_cast<const char*>(buffer) + written, size - written);
		if (bytes >= 0) {
			written += bytes;
		} else if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
			continue;
		} else {
			throw SocketException(runtime::GetSystemErrorMessage());
		}
	}
}

void Socket::sendFileDescriptors(const int* fds, const size_t nr) const
{
	if (nr == 0) return;

	int buf;
	struct iovec iov = {
		.iov_base = &buf,
		.iov_len = sizeof(char)
	};

	char buffer[CMSG_SPACE(sizeof(int) * nr)];
	::memset(buffer, 0, sizeof(buffer));

	struct msghdr msgh;
	::memset(&msgh, 0, sizeof(msgh));

	msgh.msg_iov = &iov;
	msgh.msg_iovlen = 1;
	msgh.msg_control = buffer;
	msgh.msg_controllen = sizeof(buffer);

	struct cmsghdr *cmhp;
	cmhp = CMSG_FIRSTHDR(&msgh);
	cmhp->cmsg_level = SOL_SOCKET;
	cmhp->cmsg_type = SCM_RIGHTS;
	cmhp->cmsg_len = CMSG_LEN(sizeof(int) * nr);

	::memcpy(CMSG_DATA(cmhp), fds, sizeof(int) * nr);

	int written = 0;
	while (written < 1) {
		ssize_t ret = ::sendmsg(socketFd, &msgh, MSG_NOSIGNAL);
		if (ret >= 0) {
			written += ret;
		} else if ((errno != EAGAIN) && (errno != EWOULDBLOCK) && (errno != EINTR)) {
			throw SocketException(runtime::GetSystemErrorMessage());
		}
	}
}

void Socket::receiveFileDescriptors(int* fds, const size_t nr) const
{
	if (nr == 0) return;

	char buf = '!';
	struct iovec iov = {
		.iov_base = &buf,
		.iov_len = sizeof(char)
	};

	char buffer[CMSG_SPACE(sizeof(int) * nr) + CMSG_SPACE(sizeof(struct ucred))];
	::memset(buffer, 0, sizeof(buffer));

	struct msghdr msgh;
	::memset(&msgh, 0, sizeof(msgh));

	msgh.msg_iov = &iov;
	msgh.msg_iovlen = 1;
	msgh.msg_control = buffer;
	msgh.msg_controllen = sizeof(buffer);

	ssize_t bytes = 0;
	while (bytes < 1) {
		ssize_t ret = ::recvmsg(socketFd, &msgh, MSG_WAITALL);
		if (ret >= 0) {
			bytes += ret;
		} else {
			if ((errno != EAGAIN) && (errno != EWOULDBLOCK) && (errno != EINTR)) {
				throw SocketException(runtime::GetSystemErrorMessage());
			}
		}
	}

	int i = 0;
	for (struct cmsghdr *cmhp = CMSG_FIRSTHDR(&msgh); cmhp != NULL; cmhp = CMSG_NXTHDR(&msgh, cmhp)) {
		if ((cmhp->cmsg_level == SOL_SOCKET) && (cmhp->cmsg_type == SCM_RIGHTS)) {
			if (cmhp->cmsg_len != CMSG_LEN(sizeof(int) * nr)) {
				std::cout << "Invalid File Descriptor Table" << std::endl;
			}

			fds[i++] = *(reinterpret_cast<int*>(CMSG_DATA(cmhp)));
		}
	}
}

int Socket::createSystemdSocket(const std::string& path)
{
	int n = ::sd_listen_fds(-1);
	if (n < 0) {
		throw SocketException("sd_listen_fds faield");
	}

	for (int fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + n; ++fd) {
		if (::sd_is_socket_unix(fd, SOCK_STREAM, 1, path.c_str(), 0) > 0) {
			setCloseOnExec(fd);
			return fd;
		}
	}

	return -1;
}

int Socket::createRegularSocket(const std::string& path)
{
	if (path.size() >= sizeof(sockaddr_un::sun_path)) {
		throw SocketException(runtime::GetSystemErrorMessage(ENAMETOOLONG));
	}

	int sockfd = ::socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd == -1) {
		throw SocketException(runtime::GetSystemErrorMessage());
	}

	setCloseOnExec(sockfd);

	::sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	::strncpy(addr.sun_path, path.c_str(), sizeof(sockaddr_un::sun_path));

	if (addr.sun_path[0] == '@') {
		addr.sun_path[0] = '\0';
	} else {
		unlink(path.c_str());
	}

	if (::bind(sockfd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(struct sockaddr_un)) == -1) {
		::close(sockfd);
		throw SocketException(runtime::GetSystemErrorMessage());
	}

	int optval = 1;
	if (::setsockopt(sockfd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) == -1) {
		::close(sockfd);
		throw SocketException(runtime::GetSystemErrorMessage());
	}

	if (::listen(sockfd, MAX_BACKLOG_SIZE) == -1) {
		::close(sockfd);
		throw SocketException(runtime::GetSystemErrorMessage());
	}

	return sockfd;
}

Socket Socket::create(const std::string& path, bool activation)
{
	int fd = -1;

	if (activation)
		fd = createSystemdSocket(path);

	if (fd == -1)
		fd = createRegularSocket(path);

	return Socket(fd);
}

Socket Socket::connect(const std::string& path)
{
	if (path.size() >= sizeof(sockaddr_un::sun_path)) {
		throw SocketException(runtime::GetSystemErrorMessage(ENAMETOOLONG));
	}

	int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) {
		throw SocketException(runtime::GetSystemErrorMessage());
	}

	setCloseOnExec(fd);

	sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	::strncpy(addr.sun_path, path.c_str(), sizeof(sockaddr_un::sun_path));

	if (addr.sun_path[0] == '@') {
		addr.sun_path[0] = '\0';
	}

	if (::connect(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(struct sockaddr_un)) == -1) {
		::close(fd);
		throw SocketException(runtime::GetSystemErrorMessage());
	}

	return Socket(fd);
}

} // namespace rmi
} // namespace klay
