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
#include <limits>

#include <klay/error.h>
#include <klay/exception.h>
#include <klay/audit/logger.h>
#include <klay/netlink/netlink.h>

namespace klay {
namespace netlink {

Netlink::Netlink(int protocol) :
	sequence(1)
{
	fd = ::socket(PF_NETLINK, SOCK_RAW, protocol);
	if (fd < 0) {
		throw klay::Exception(klay::GetSystemErrorMessage());
	}

	if (::fcntl(fd, F_SETFD, FD_CLOEXEC) == -1) {
		::close(fd);
		throw klay::Exception(klay::GetSystemErrorMessage());
	}
}

Netlink::Netlink(Netlink&& netlink) :
	fd(netlink.fd), sequence(netlink.sequence)
{
	netlink.fd = -1;
}

Netlink::~Netlink()
{
	if (fd > 0) {
		::close(fd);
	}
}

void Netlink::send(int type, const std::vector<char>& data)
{
	struct sockaddr_nl addr;
	int ret;

	::memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_pid = 0;
	addr.nl_groups = 0;

	char buf[NLMSG_SPACE(data.size())];
	auto *nlh = (struct nlmsghdr *)buf;

	::memset(nlh, 0, sizeof(*nlh));

	nlh->nlmsg_len = sizeof(buf);
	nlh->nlmsg_type = type;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = sequence++;

	// overflow - The sequence of message from kernel is 0,
	// so this is not to use 0 as a sequence number.
	if (sequence == 0)
		sequence++;

	::memcpy(NLMSG_DATA(buf), data.data(), data.size());


	do {
		ret = ::sendto(fd, buf, sizeof(buf), 0,
						(struct sockaddr*)&addr, sizeof(addr));
	} while (ret < 0 && errno == EINTR);

	if (ret < 0) {
		throw klay::Exception("Failed to send a netlink message");
	}

	if (recv(MSG_PEEK).first == NLMSG_ERROR) {
		auto reply = recv().second;
		auto err = (struct nlmsgerr*)reply.data();
		if (err->error)
			throw klay::Exception("Netlink error: " +
										std::to_string(err->error));
	} else {
			WARN("Missing ack of netlink message");
	}
}

Netlink::Message Netlink::recv(int options)
{
	int ret;

	//To get message size
	struct nlmsghdr nlh;
	do {
		ret = ::recv(fd, &nlh, sizeof(nlh), options | MSG_PEEK);
	} while (ret < 0 && errno == EINTR);

	if (ret < 0) {
		throw klay::Exception("Failed to get the size of netlink message");
	}

	struct sockaddr_nl nladdr;
	socklen_t nladdrlen = sizeof(nladdr);

	if (nlh.nlmsg_len > (std::numeric_limits<decltype(nlh.nlmsg_len)>::max() - NLMSG_HDRLEN))
		throw klay::Exception("Netlink message is too large.");

	char buf[nlh.nlmsg_len + NLMSG_HDRLEN];
	do {
		ret = ::recvfrom(fd, buf, sizeof(buf), options,
							(struct sockaddr*)&nladdr, &nladdrlen);
	} while (ret < 0 && errno == EINTR);

	Message msg = {nlh.nlmsg_type, std::vector<char>(nlh.nlmsg_len)};

	::memcpy(msg.second.data(), NLMSG_DATA(buf), msg.second.size());

	if (ret < 0) {
		throw klay::Exception("Failed to receive audit packet");
	}

	if (nladdrlen != sizeof(nladdr)) {
		throw klay::Exception("Bad address size in netlink socket");
	}

	if (nladdr.nl_pid) {
		throw klay::Exception("Spoofed packet received on netlink socket");
	}

	return msg;
}

} // namespace klay
} // namespace klay
