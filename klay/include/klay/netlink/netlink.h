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

#ifndef __RUNTIME_NETLINK_NETLINK_H__
#define __RUNTIME_NETLINK_NETLINK_H__

#include <linux/netlink.h>
#include <vector>

#include <klay/klay.h>

#ifndef NETLINK_AUDIT
#define NETLINK_AUDIT 9
#endif

namespace klay {
namespace netlink {

class KLAY_EXPORT Netlink final {
public:
	typedef std::pair<int, std::vector<char>> Message;

	Netlink(int);
	Netlink(Netlink&&);
	Netlink(const Netlink&) = delete;
	~Netlink();

	Netlink& operator=(const Netlink&) = delete;

	void send(int type, const std::vector<char>& data);
	Message recv(int options = 0);

	int getFd() const {
		return fd;
	}

private:
	int fd;
	unsigned int sequence;
};

} // namespace netlink
} // namespace klay

namespace netlink = klay::netlink;

#endif //!__RUNTIME_NETLINK_NETLINK_H__
