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

#include <string>
#include <memory>

#include <klay/rmi/client.h>

namespace vist {
namespace ipc {

class Client final {
public:
	Client(const Client&) = delete;
	Client& operator=(const Client&) = delete;

	Client(Client&&) = delete;
	Client& operator=(Client&&) = delete;

	static std::unique_ptr<klay::rmi::Client>& Instance(const std::string& sock)
	{
		static Client client(sock);
		return client.instance;
	}

private:
	explicit Client(const std::string& sock) :
		instance(std::make_unique<klay::rmi::Client>(sock))
	{
		instance->connect();
	}
	~Client() = default;

	std::unique_ptr<klay::rmi::Client> instance;
};

} // namespace ipc
} // namespace vist
