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

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <klay/exception.h>
#include <klay/rmi/notification.h>

namespace klay {
namespace rmi {

Notification::Notification()
{
}

Notification::Notification(const std::string& name) :
	signalName(name)
{
}

Notification::Notification(Notification&& rhs) :
	signalName(std::move(rhs.signalName)),
	subscribers(std::move(rhs.subscribers))
{
}

SubscriptionId Notification::createSubscriber()
{
	int fds[2] = {-1, -1};
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1) {
		throw runtime::Exception("Failed to create socket pair");
	}

	std::lock_guard<std::mutex> lock(subscriberLock);
	subscribers.push_back(std::make_shared<Socket>(fds[0]));

	return SubscriptionId(fds[0], fds[1]);
}

int Notification::removeSubscriber(const int id)
{
	std::lock_guard<std::mutex> lock(subscriberLock);

	std::list<std::shared_ptr<Socket>>::iterator it = subscribers.begin();

	while (it != subscribers.end()) {
	   if ((*it)->getFd() == id) {
			subscribers.erase(it);
			return 0;
	   }
	   ++it;
	}

	return -1;
}

} // namespace rmi
} // namespace klay
