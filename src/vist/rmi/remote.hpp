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
 * @file   remote.hpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @brief  Client-side stub for invoking remote method.
 */

#pragma once

#include <vist/transport/message.hpp>

#include <string>
#include <memory>

namespace vist {
namespace rmi {

using namespace vist::transport;

class Remote {
public:
	explicit Remote(const std::string& path);

	template<typename R, typename... Args>
	R invoke(const std::string& name, Args&&... args);

private:
	Message request(Message message);

	class Impl;
	/// Let compiler know how to destroy Impl
	struct ImplDeleter
	{
		void operator()(Impl*);
	};

	std::unique_ptr<Impl, ImplDeleter> pImpl;
};

template<typename R, typename... Args>
R Remote::invoke(const std::string& method, Args&&... args)
{
	Message message(Message::Type::MethodCall, method);
	message.enclose(std::forward<Args>(args)...);

	Message reply = this->request(message);
	R result;
	reply.disclose(result);

	return result;
}

} // namespace rmi
} // namespace vist
