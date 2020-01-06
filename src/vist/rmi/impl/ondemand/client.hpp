/*
 *  Copyright (c) 2020 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/logger.hpp>
#include <vist/rmi/impl/client.hpp>
#include <vist/rmi/impl/ondemand/connection.hpp>

namespace vist {
namespace rmi {
namespace impl {
namespace ondemand {

class Client : public interface::Client {
public:
	Client(const std::string& path) : interface::Client(path), connection(path)
	{
		DEBUG(VIST) << "Success to connect to : " << path
					<< " by fd[" << connection.getFd() << "]";
	}

	Message request(Message& message) override
	{
		return this->connection.request(message);
	}

private:
	Connection connection;
};

} // namespace ondemand
} // namespace impl
} // namespace rmi
} // namespace vist
