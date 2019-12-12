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
 * @file   remote.cpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @brief  Implementation of remote. 
 */

#include "remote.hpp"

#include <vist/transport/client.hpp>

#include <string>
#include <mutex>

namespace vist {
namespace rmi {

using namespace vist::transport;

class Remote::Impl {
public:
	explicit Impl(const std::string& path) : client(path)
	{
	}

	Message request(Message message)
	{
		return this->client.request(message);
	}

private:
	Client client;
};

Remote::Remote(const std::string& path) : pImpl(new Impl(path))
{
}

Message Remote::request(Message message)
{
	return pImpl->request(message);
}

void Remote::ImplDeleter::operator()(Impl* ptr)
{
	delete ptr;
}

} // namespace rmi
} // namespace vist
