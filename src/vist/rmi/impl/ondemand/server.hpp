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

#include <vist/rmi/impl/server.hpp>
#include <vist/rmi/impl/ondemand/connection.hpp>
#include <vist/rmi/impl/ondemand/mainloop.hpp>
#include <vist/rmi/impl/ondemand/socket.hpp>

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>

namespace vist {
namespace rmi {
namespace impl {
namespace ondemand {

class Server : public interface::Server {
public:
	Server(const std::string& path, const interface::Task& task) :
		interface::Server(path, task),
		socket(path)
	{
		this->accept(task);
	}

	virtual ~Server() = default;

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	Server(Server&&) = default;
	Server& operator=(Server&&) = default;

	void run(void) override
	{
		this->mainloop.run();
	}

	void stop(void) override
	{
		this->mainloop.removeHandler(this->socket.getFd());
		this->mainloop.stop();
	}

private:
	void accept(const interface::Task& task) override
	{
		auto handler = [this, task]() {
			DEBUG(VIST) << "New session is accepted.";

			auto connection = std::make_shared<Connection>(this->socket.accept());
			auto onRead = [connection, task]() {
				Message request = connection->recv();
				DEBUG(VIST) << "Session header: " << request.signature;

				try {
					Message reply = task(request);
					connection->send(reply);
				} catch (const std::exception& e) {
					ERROR(VIST) << e.what();
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
		this->mainloop.addHandler(this->socket.getFd(), std::move(handler));
	}

	Socket socket;
	Mainloop mainloop;
};

} // namespace ondemand
} // namespace impl
} // namespace rmi
} // namespace vist
