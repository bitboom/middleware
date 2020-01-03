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

#include "connection.hpp"

#include <vist/logger.hpp>

#include <utility>

namespace vist {
namespace rmi {
namespace impl {
namespace ondemand {

Connection::Connection(Socket&& socket) noexcept : socket(std::move(socket))
{
}

Connection::Connection(const std::string& path) :
	socket(Socket::connect(path))
{
	DEBUG(VIST) << "Connect to " << path << " by fd: " << socket.getFd();
}

void Connection::send(Message& message)
{
	std::lock_guard<std::mutex> lock(this->sendMutex);

	message.header.id = this->sequence++;
	this->socket.send(&message.header);

	this->socket.send(message.getBuffer().data(), message.header.length);
}

Message Connection::recv(void) const
{
	std::lock_guard<std::mutex> lock(this->recvMutex);
	Message::Header header;
	this->socket.recv(&header);

	Message message(header);
	this->socket.recv(message.getBuffer().data(), message.size());
	message.disclose(message.signature);

	return message;
}

Message Connection::request(Message& message)
{
	this->send(message);
	return this->recv();
}

int Connection::getFd(void) const noexcept
{
	return this->socket.getFd();
}

} // namespace ondemand
} // namespace impl
} // namespace rmi
} // namespace vist
