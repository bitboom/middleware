/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include <cstring>

#include <klay/error.h>
#include <klay/exception.h>
#include <klay/netlink/netlink.h>

namespace netlink {

Netlink::Netlink(int protocol)
{
	fd = ::socket(PF_NETLINK, SOCK_RAW, protocol);
	if (fd < 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	if (::fcntl(fd, F_SETFD, FD_CLOEXEC) == -1) {
		::close(fd);
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}
}

Netlink::Netlink(Netlink&& netlink) :
	fd(netlink.fd)
{
	netlink.fd = -1;
}

Netlink::~Netlink()
{
	if (fd > 0) {
		::close(fd);
	}
}

void Netlink::send(const void *buf, unsigned int size)
{
	struct sockaddr_nl addr;
	int ret;

	::memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_pid = 0;
	addr.nl_groups = 0;

	do {
		ret = ::sendto(fd, buf, size, 0,
						(struct sockaddr*)&addr, sizeof(addr));
	} while (ret < 0 && errno == EINTR);

	if (ret < 0) {
		throw runtime::Exception("Failed to send audit packet");
	}
}

void Netlink::recv(void *buf, unsigned int size, int options)
{
	struct sockaddr_nl nladdr;
	socklen_t nladdrlen = sizeof(nladdr);
	int ret;

	do {
		ret = ::recvfrom(fd, buf, size, options,
							(struct sockaddr*)&nladdr, &nladdrlen);
	} while (ret < 0 && errno == EINTR);

	if (ret < 0) {
		throw runtime::Exception("Failed to receive audit packet");
	}

	if (nladdrlen != sizeof(nladdr)) {
		throw runtime::Exception("Bad address size reading audit netlink socket");
	}

	if (nladdr.nl_pid) {
		throw runtime::Exception("Spoofed packet received on audit netlink socket");
	}
}

} // namespace runtime
