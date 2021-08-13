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

#include <utility>

#include <klay/rmi/connection.h>

namespace klay {
namespace rmi {

Connection::Connection(Socket&& sock) :
	socket(std::move(sock))
{
}

Connection::Connection(const std::string &address) :
	socket(Socket::connect(address))
{
}

Connection::~Connection() noexcept
{
}

Message Connection::createMessage(unsigned int type, const std::string& target)
{
	return Message(type, target);
}

void Connection::send(const Message& message) const
{
	std::lock_guard<std::mutex> lock(transmitMutex);
	message.encode(socket);
}

Message Connection::dispatch() const
{
	Message message;
	std::lock_guard<std::mutex> lock(receiveMutex);

	message.decode(socket);

	return message;
}

} // namespace rmi
} // namespace klay
