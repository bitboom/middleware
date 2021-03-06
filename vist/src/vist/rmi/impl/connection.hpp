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

#pragma once

#include <vist/rmi/message.hpp>
#include <vist/rmi/impl/socket.hpp>

#include <mutex>
#include <utility>

namespace vist {
namespace rmi {
namespace impl {

class Connection {
public:
	explicit Connection(Socket&& socket) noexcept;
	explicit Connection(const std::string& path);
	virtual ~Connection() = default;

	Connection(const Connection&) = delete;
	Connection& operator=(const Connection&) = delete;

	Connection(Connection&&) = default;
	Connection& operator=(Connection&&) = default;

	// server-side
	void send(Message& message);
	Message recv(void) const;

	// client-side
	Message request(Message& message);

	int getFd(void) const noexcept;

private:
	Socket socket;

	// SOCK_STREAM are full-duplex byte streams
	mutable std::mutex sendMutex;
	mutable std::mutex recvMutex;

	unsigned int sequence = 0;
};

} // namespace impl
} // namespace rmi
} // namespace vist
