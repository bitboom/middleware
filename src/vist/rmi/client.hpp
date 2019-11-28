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
/*
 * @file   client.hpp
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Client application for invoking remote function(method).
 */

#pragma once

#include <vist/transport/connection.hpp>
#include <vist/transport/message.hpp>

#include <string>
#include <mutex>

using namespace vist::transport;

namespace vist {
namespace rmi {

class Client {
public:
	explicit Client(const std::string& remotePath);
	virtual ~Client() = default;

	Client(const Client&) = delete;
	Client& operator=(const Client&) = delete;

	Client(Client&&) = delete;
	Client& operator=(Client&&) = delete;

	template<typename R, typename... Args>
	R invoke(const std::string& name, Args&&... args);

private:
	Connection connection;
	std::mutex mutex;
};

template<typename R, typename... Args>
R Client::invoke(const std::string& name, Args&&... args)
{
	Message msg(Message::Type::MethodCall, name);
	msg.enclose(std::forward<Args>(args)...);

	std::lock_guard<std::mutex> lock(this->mutex);

	Message reply = this->connection.request(msg);
	R ret;
	reply.disclose(ret);

	return ret;
}

} // namespace rmi
} // namespace vist
