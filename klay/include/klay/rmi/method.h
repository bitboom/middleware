/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __RMI_REMOTEMETHOD_H__
#define __RMI_REMOTEMETHOD_H__

#include <thread>
#include <string>
#include <memory>
#include <iostream>

#include <klay/klay.h>
#include <klay/rmi/client.h>
#include <klay/rmi/message.h>
#include <klay/rmi/connection.h>
#include <klay/rmi/callback-holder.h>

namespace klay {
namespace rmi {

template <typename ExceptionModel = DefaultExceptionModel>
class KLAY_EXPORT RemoteMethod {
public:
	RemoteMethod() = delete;
	virtual ~RemoteMethod();

	RemoteMethod(const RemoteMethod&) = delete;
	RemoteMethod& operator=(const RemoteMethod&) = delete;

	template<typename Type, typename... Args>
	static Type invoke(Connection &connection, const std::string& method, Args&&... args)
	{
		Message request = connection.createMessage(Message::MethodCall, method);
		request.packParameters(std::forward<Args>(args)...);
		connection.send(request);

		Type response;
		Message reply = connection.dispatch();
		if (reply.isError()) {
			std::string klass;
			reply.disclose(klass);
			ExceptionModel exception;
			exception.raise(reply.target(), klass);
		}

		reply.disclose<Type>(response);

		return response;
	}
};

template <typename ExceptionModel>
RemoteMethod<ExceptionModel>::~RemoteMethod()
{
}

} // namespace rmi
} // namespace klay

namespace rmi = klay::rmi;

#endif //__RMI_REMOTEMETHOD_H__
