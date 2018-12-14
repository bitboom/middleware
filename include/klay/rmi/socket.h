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

#ifndef __RMI_SOCKET_H__
#define __RMI_SOCKET_H__

#include <klay/klay.h>
#include <klay/exception.h>

namespace klay {
namespace rmi {

class KLAY_EXPORT SocketException: public runtime::Exception {
public:
	SocketException(const std::string& msg) : runtime::Exception(msg) {}
};

struct KLAY_EXPORT Credentials {
	pid_t pid;
	uid_t uid;
	gid_t gid;
	std::string security;
};

class KLAY_EXPORT Socket {
public:
	explicit Socket(int socketFd = -1, bool autoclose = true);
	Socket(Socket&& socket) noexcept;
	~Socket() noexcept;

	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
	Socket& operator=(Socket&) = delete;

	Socket accept();
	int getFd() const;
	Credentials getPeerCredentials() const;

	void write(const void* buffer, const size_t size) const;
	void read(void* buffer, const size_t size) const;

	void sendFileDescriptors(const int* fds, const size_t nr) const;
	void receiveFileDescriptors(int* fds, const size_t nr) const;

	static Socket create(const std::string& path, bool activation = false);
	static Socket connect(const std::string& path);

private:
	static int createRegularSocket(const std::string& path);

	static int createSystemdSocket(const std::string& path);

private:
	int socketFd;
	bool autoClose;
};

} // namespace rmi
} // namespace klay

namespace rmi = klay::rmi;

#endif //__RMI_SOCKET_H__
