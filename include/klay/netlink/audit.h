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

#ifndef __RUNTIME_NETLINK_AUDIT_H__
#define __RUNTIME_NETLINK_AUDIT_H__

#include <list>
#include <vector>

#include <linux/audit.h>

#include <klay/mainloop.h>
#include <klay/netlink/netlink.h>

namespace netlink {

class Audit final {
public:
	typedef std::pair<int, std::vector<char>> Message;
	typedef std::function<void(Message&)> MessageHandler;

	Audit();
    Audit(Audit&&);
    Audit(const Audit&) = delete;
    ~Audit();

    Audit& operator=(const Audit&) = delete;

	void setPID(pid_t pid);
	void setEnabled(int enabled);
	int isEnabled();

	void setMainloop(runtime::Mainloop *ml);
	void setMessageHandler(MessageHandler&& handler);

private:
	void send(int type, const void *data, unsigned int size);
	Message recv(int options = 0);

	void processMessage(int fd, runtime::Mainloop::Event event);

	Netlink nl;
	unsigned int sequence = 0;
	runtime::Mainloop *mainloop;
	MessageHandler messageHandler;
};

} // namespace runtime

#endif //!__RUNTIME_NETLINK_AUDIT_H__
