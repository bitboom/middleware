/*
 *  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include "gateway.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>
#include <vist/rmi/message.hpp>
#include <vist/rmi/impl/server.hpp>

#include <memory>
#include <string>

namespace vist {
namespace rmi {

using namespace vist::rmi::impl;

class Gateway::Impl {
public:
	explicit Impl(Gateway& gateway, const std::string& path, Gateway::ServiceType type)
	{
		auto dispatcher = [&gateway](auto & message) -> Message {
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

		this->server = std::make_unique<Server>(path, dispatcher, type);
	}

	inline void start(int timeout, std::function<bool()> stopper)
	{
		this->server->run(timeout, stopper);
	}

	inline void stop()
	{
		this->server->stop();
	}

private:
	std::unique_ptr<Server> server;
};

Gateway::Gateway(const std::string& path, ServiceType type) :
	pImpl(std::make_unique<Impl>(*this, path, type))
{
}

Gateway::~Gateway() = default;

void Gateway::start(int timeout, std::function<bool()> stopper)
{
	this->pImpl->start(timeout, stopper);
}

void Gateway::stop(void)
{
	this->pImpl->stop();
}

/// Credentials exists per thread.
std::shared_ptr<Credentials> Gateway::GetPeerCredentials() noexcept
{
	return Server::GetPeerCredentials();
}

} // namespace rmi
} // namespace vist
