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

#include <vist/timer.hpp>
#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include <errno.h>
#include <unistd.h>

namespace vist {
namespace rmi {
namespace impl {

class Server {
public:
	Server(const std::string& path, const Protocol::Task& task) : polling(false)
	{
		errno = 0;
		if (::unlink(path.c_str()) == -1 && errno != ENOENT)
			THROW(ErrCode::RuntimeError) << "Failed to remove file."; 

		this->acceptor = std::make_unique<Protocol::Acceptor>(this->context,
															  Protocol::Endpoint(path));
		this->accept(task);
	}

	inline void accept(const Protocol::Task& task)
	{
		auto asyncSession = std::make_shared<Protocol::Async>(this->context);
		auto handler = [this, asyncSession, task](const auto& error) {
			DEBUG(VIST) << "New session is accepted.";

			if (error)
				THROW(ErrCode::RuntimeError) << error.message();

			asyncSession->dispatch(task, this->polling);

			this->accept(task);
		};
		this->acceptor->async_accept(asyncSession->getSocket(), handler);
	}

	inline void run(unsigned int timeout = 0, Timer::Predicate condition = nullptr)
	{
		if (timeout > 0) {
			auto stopper = [this]() {
				INFO(VIST) << "There are no sessions. And timeout is occured.";
				this->context.stop();
			};

			auto wrapper = [this, condition]() -> bool {
				if (condition)
					return condition() && polling == false;

				return polling == false;
			};

			Timer::ExecOnce(stopper, wrapper, timeout);
		}

		this->context.run();
	}

	inline void stop()
	{
		this->context.stop();
	}

private:
	Protocol::Context context;
	std::unique_ptr<Protocol::Acceptor> acceptor;

	/// check for session is maintained
	std::atomic<bool> polling;
};

} // namespace impl
} // namespace rmi
} // namespace vist
