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

#include <vist/rmi/gateway.hpp>
#include <vist/rmi/impl/connection.hpp>
#include <vist/rmi/impl/mainloop.hpp>
#include <vist/rmi/impl/socket.hpp>
#include <vist/rmi/impl/systemd-socket.hpp>

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <functional>

namespace vist {
namespace rmi {
namespace impl {

using Task = std::function<Message(Message&)>;
using Stopper = std::function<bool(void)>;
using ServiceType = Gateway::ServiceType;

class Server {
public:
	Server(const std::string& path, const Task& task, ServiceType type = ServiceType::General)
	{
		switch (type) {
		case ServiceType::OnDemand:
			this->socket = std::make_unique<Socket>(SystemdSocket::Create(path));
			break;
		case ServiceType::General: /// fall through
		default:
			this->socket = std::make_unique<Socket>(path);
			break;
		}

		this->accept(task);
	}

	virtual ~Server() = default;

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	Server(Server&&) = default;
	Server& operator=(Server&&) = default;

	void run(int timeout = -1, Stopper stopper = nullptr)
	{
		this->mainloop.run(timeout, stopper);
	}

	void stop(void)
	{
		this->mainloop.removeHandler(this->socket->getFd());
		this->mainloop.stop();
	}

private:
	void accept(const Task& task)
	{
		auto handler = [this, task]() {
			DEBUG(VIST) << "New session is accepted.";

			auto connection = std::make_shared<Connection>(this->socket->accept());
			auto onRead = [connection, task]() {
				Message request = connection->recv();
				DEBUG(VIST) << "Session header: " << request.signature;

				try {
					Message reply = task(request);
					connection->send(reply);
				} catch (const std::exception& e) {
					ERROR(VIST) << e.what();
					Message reply = Message(Message::Type::Error, e.what());
					connection->send(reply);
				}
			};

			auto onClose = [this, connection]() {
				DEBUG(VIST) << "Connection closed. fd: " << connection->getFd();
				this->mainloop.removeHandler(connection->getFd());
			};

			this->mainloop.addHandler(connection->getFd(),
									std::move(onRead), std::move(onClose));
		};

		INFO(VIST) << "Ready for new connection.";
		this->mainloop.addHandler(this->socket->getFd(), std::move(handler));
	}

	std::unique_ptr<Socket> socket;
	Mainloop mainloop;
};

} // namespace impl
} // namespace rmi
} // namespace vist
