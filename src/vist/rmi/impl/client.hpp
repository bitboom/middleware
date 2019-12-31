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

namespace vist {
namespace rmi {
namespace impl {

using boost::asio::local::stream_protocol;

class Client {
public:
	Client(const std::string& path) : socket(this->context)
	{
		try {
			this->socket.connect(Protocol::Endpoint(path));
		}  catch(const std::exception& e) {
			ERROR(VIST) << "Failed to connect socket: " << e.what();
			std::rethrow_exception(std::current_exception());
		}
	}

	inline Message request(Message& message)
	{
		return Protocol::Request(this->socket, message);
	}

private:
	Protocol::Context context;
	Protocol::Socket socket;
};

} // namespace impl
} // namespace rmi
} // namespace vist
