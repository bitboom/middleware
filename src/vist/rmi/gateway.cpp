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
/*
 * @file   gateway.cpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @brief  Implementation of Server-side stub for exposing method. 
 */

#include "gateway.hpp"

#include <vist/exception.hpp>
#include <vist/transport/message.hpp>
#include <vist/transport/server.hpp>

#include <string>

namespace vist {
namespace rmi {

using namespace vist::transport;

class Gateway::Impl {
public:
	explicit Impl(Gateway& gateway, const std::string& path)
	{
		auto dispatcher = [&gateway](auto& message) -> Message {
			std::string function = message.signature;
			auto iter = gateway.functorMap.find(function);
			if (iter == gateway.functorMap.end())
				THROW(ErrCode::ProtocolBroken) << "Not found function: " << function;

			DEBUG(VIST) << "Remote method invokation: " << function;

			auto functor = iter->second;
			auto result = functor->invoke(message.buffer);

			Message reply(Message::Type::Reply, function);
			reply.enclose(result);

			return reply;
		};

		this->server = std::make_unique<Server>(path, dispatcher);
	}

	inline void start(unsigned int timeout, Timer::Predicate condition)
	{
		this->server->run(timeout, condition);
	}

	inline void stop()
	{
		this->server->stop();
	}

private:
	std::unique_ptr<Server> server;
};

Gateway::Gateway(const std::string& path) : pImpl(std::make_unique<Impl>(*this, path))
{
}

Gateway::~Gateway() = default;

void Gateway::start(unsigned int timeout, Timer::Predicate condition)
{
	this->pImpl->start(timeout, condition);
}

void Gateway::stop(void)
{
	this->pImpl->stop();
}

} // namespace rmi
} // namespace vist
