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

#pragma once

#include <vist/exception.hpp>
#include <vist/rmi/message.hpp>

#include <string>
#include <memory>

#define REMOTE_METHOD(remote, method) vist::rmi::Remote::Method {remote, #method}

namespace vist {
namespace rmi {

class Remote final {
public:
	explicit Remote(const std::string& path);
	~Remote();

	Remote(const Remote&) = delete;
	Remote& operator=(const Remote&) = delete;

	Remote(Remote&&) = default;
	Remote& operator=(Remote&&) = default;

	template<typename R, typename... Args>
	R invoke(const std::string& name, Args&&... args);

	struct Method {
		Remote& remote;
		std::string name;

		template<typename R, typename... Args>
		R invoke(Args&&... args)
		{
			return remote.invoke<R>(name, std::forward<Args>(args)...);
		}
	};

private:
	Message request(Message& message);

	class Impl;
	std::unique_ptr<Impl> pImpl;
};

template<typename R, typename... Args>
R Remote::invoke(const std::string& method, Args&&... args)
{
	Message message(Message::Type::MethodCall, method);
	message.enclose(std::forward<Args>(args)...);

	Message reply = this->request(message);
	if (reply.error())
		THROW(ErrCode::RuntimeError) << reply.what();

	R result;
	reply.disclose(result);

	return result;
}

} // namespace rmi
} // namespace vist
