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

#include <vist/credentials.hpp>
#include <vist/rmi/gateway.hpp>
#include <vist/rmi/impl/mainloop.hpp>
#include <vist/rmi/impl/socket.hpp>
#include <vist/rmi/message.hpp>
#include <vist/thread-pool.hpp>

#include <memory>
#include <string>
#include <functional>

namespace vist {
namespace rmi {
namespace impl {

using Task = std::function<Message(Message&)>;
using Stopper = std::function<bool(void)>;
using ServiceType = Gateway::ServiceType;

class Server {
public:
	Server(const std::string& path, const Task& task, ServiceType type = ServiceType::General);
	virtual ~Server() = default;

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	Server(Server&&) = default;
	Server& operator=(Server&&) = default;

	void run(int timeout = -1, Stopper stopper = nullptr);
	void stop(void);

	static std::shared_ptr<Credentials> GetPeerCredentials()
	{
		return peer;
	}

private:
	void accept(const Task& task);

	static thread_local std::shared_ptr<Credentials> peer;

	std::unique_ptr<Socket> socket;
	Mainloop mainloop;
	ThreadPool worker;
};

} // namespace impl
} // namespace rmi
} // namespace vist

