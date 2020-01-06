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

#include "remote.hpp"

#include <vist/rmi/impl/client.hpp>

#ifdef TIZEN
#include <vist/rmi/impl/ondemand/client.hpp>
#else
#include <vist/rmi/impl/general/client.hpp>
#endif

#include <memory>
#include <mutex>
#include <string>

namespace vist {
namespace rmi {

using namespace vist::rmi::impl;

class Remote::Impl {
public:
	explicit Impl(const std::string& path)
	{
#ifdef TIZEN
		this->client = std::make_unique<ondemand::Client>(path);
#else
		this->client = std::make_unique<general::Client>(path);
#endif
	}

	Message request(Message& message)
	{
		return this->client->request(message);
	}

private:
	std::unique_ptr<interface::Client> client;
};

Remote::Remote(const std::string& path) : pImpl(new Impl(path))
{
}

Remote::~Remote() = default;

Message Remote::request(Message& message)
{
	return pImpl->request(message);
}

} // namespace rmi
} // namespace vist
