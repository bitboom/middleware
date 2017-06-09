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
#include <iostream>

#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#include <cstring>

#include <klay/error.h>
#include <klay/exception.h>

#include <klay/netlink/audit.h>

using namespace std::placeholders;

namespace netlink {

Audit::Audit() :
	nl(NETLINK_AUDIT), sequence(0), mainloop(nullptr)
{

}

Audit::Audit(Audit&& audit) :
	nl(std::move(audit.nl)), sequence(audit.sequence), mainloop(audit.mainloop)
{

}

Audit::~Audit()
{

}

void Audit::setPID(pid_t pid)
{
	struct audit_status s;

	::memset(&s, 0, sizeof(s));
	s.mask	= AUDIT_STATUS_PID;
	s.pid	= pid;

	send(AUDIT_SET, &s, sizeof(s));
}

void Audit::setEnabled(int enabled)
{
	struct audit_status s;

	::memset(&s, 0, sizeof(s));
	s.mask		= AUDIT_STATUS_ENABLED;
	s.enabled	= enabled;

	send(AUDIT_SET, &s, sizeof(s));
}

int Audit::isEnabled()
{
	int ret = 0;
	send(AUDIT_GET, NULL, 0);

	while (1) {
		auto msg = recv();

		switch (msg.first) {
		case NLMSG_DONE:
		case NLMSG_ERROR:
			throw runtime::Exception("Failed to get audit state");
		case AUDIT_GET:
			ret = ((struct audit_status*)msg.second.data())->enabled;
			break;
		default:
			continue;
		}

		break;
	}

	return ret;
}

void Audit::setMainloop(runtime::Mainloop *ml)
{
	int fd = nl.getFd();

	if (mainloop != nullptr) {
		mainloop->removeEventSource(fd);
	}
	mainloop = ml;
	mainloop->addEventSource(fd, POLLIN, std::bind(&Audit::processMessage,
													this, _1, _2));
}

void Audit::setMessageHandler(MessageHandler&& handler)
{
	messageHandler = handler;
}

void Audit::send(int type, const void *data, unsigned int size)
{
	char buf[NLMSG_SPACE(size)];
	auto *nlh = (struct nlmsghdr *)buf;

	::memset(buf, 0, sizeof(buf));

	nlh->nlmsg_len = sizeof(buf);
	nlh->nlmsg_type = type;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	nlh->nlmsg_seq = ++sequence;

	if (sequence == 0) {
		sequence++;
	}

	if (size && data) {
		::memcpy(NLMSG_DATA(buf), data, size);
	}

	nl.send(&buf, sizeof(buf));

	if (recv(MSG_PEEK).first == NLMSG_ERROR) {
		auto err = (struct nlmsgerr*)recv().second.data();
		if (err->error) {
			throw runtime::Exception("Audit netlink error: " +
										std::to_string(err->error));
		}
	}
}

Audit::Message Audit::recv(int options)
{
	struct nlmsghdr nlh;

	processMessage(nl.getFd(), POLLIN);

	//To get message size
	nl.recv(&nlh, sizeof(nlh), options | MSG_PEEK);

	char buf[nlh.nlmsg_len + NLMSG_HDRLEN];
	nl.recv(buf, sizeof(buf), options);

	Message msg = {nlh.nlmsg_type, std::vector<char>(nlh.nlmsg_len)};

	::memcpy(msg.second.data(), NLMSG_DATA(buf), msg.second.size());
	return msg;
}

void Audit::processMessage(int fd, runtime::Mainloop::Event event)
{
	struct nlmsghdr nlh;

	nl.recv(&nlh, sizeof(nlh), MSG_PEEK);

	while (nlh.nlmsg_seq == 0) {
		char buf[nlh.nlmsg_len + NLMSG_HDRLEN];
		nl.recv(buf, sizeof(buf));

		Message msg = {nlh.nlmsg_type, std::vector<char>(nlh.nlmsg_len)};

		::memcpy(msg.second.data(), NLMSG_DATA(buf), msg.second.size());
		if (messageHandler) {
			messageHandler(msg);
		}

		try {
			nl.recv(&nlh, sizeof(nlh), MSG_PEEK | MSG_DONTWAIT);
		} catch (runtime::Exception &e) {
			break;
		}
	}
}

} // namespace runtime
