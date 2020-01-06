/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "protocol.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <memory>

#include <errno.h>
#include <unistd.h>

namespace vist {
namespace rmi {
namespace impl {
namespace general {

class Server : public interface::Server {
public:
	Server(const std::string& path, const interface::Task& task) : interface::Server(path, task)
	{
		errno = 0;
		if (::unlink(path.c_str()) == -1 && errno != ENOENT)
			THROW(ErrCode::RuntimeError) << "Failed to remove file."; 

		this->acceptor = std::make_unique<Protocol::Acceptor>(this->context,
															  Protocol::Endpoint(path));
		this->accept(task);
	}
	virtual ~Server() = default;

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	Server(Server&&) = default;
	Server& operator=(Server&&) = default;

	void run() override
	{
		this->context.run();
	}

	void stop() override
	{
		this->context.stop();
	}

private:
	void accept(const interface::Task& task) override
	{
		auto asyncSession = std::make_shared<Protocol::Async>(this->context);
		auto handler = [this, asyncSession, task](const auto& error) {
			DEBUG(VIST) << "New session is accepted.";

			if (error)
				THROW(ErrCode::RuntimeError) << error.message();

			asyncSession->dispatch(task);

			this->accept(task);
		};
		this->acceptor->async_accept(asyncSession->getSocket(), handler);
	}

	Protocol::Context context;
	std::unique_ptr<Protocol::Acceptor> acceptor;
};

} // namespace general
} // namespace impl
} // namespace rmi
} // namespace vist
