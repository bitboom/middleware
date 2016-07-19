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

#ifndef __RMI_CLIENT_H__
#define __RMI_CLIENT_H__

#include <thread>
#include <string>
#include <memory>
#include <iostream>

#include <klay/mainloop.h>
#include <klay/rmi/message.h>
#include <klay/rmi/connection.h>
#include <klay/rmi/callback-holder.h>

namespace rmi {

class Client {
public:
	Client(const std::string& address);
	~Client();

	Client(const Client&) = delete;
	Client& operator=(const Client&) = delete;

	void connect();
	void disconnect();

	int subscribe(const std::string& provider, const std::string& name);

	template<typename... Args>
	int subscribe(const std::string& provider, const std::string& name,
				  const typename MethodHandler<void, Args...>::type& handler);

	int unsubscribe(const std::string& provider, int handle);

	template<typename Type, typename... Args>
	Type methodCall(const std::string& method, Args&&... args);

private:
	std::string address;
	std::shared_ptr<Connection> connection;
	runtime::Mainloop mainloop;
	std::thread dispatcher;
};

template<typename... Args>
int Client::subscribe(const std::string& provider, const std::string& name,
					  const typename MethodHandler<void, Args...>::type& handler)
{
	int id = subscribe(provider, name);
	if (id < 0) {
		return -1;
	}

	std::shared_ptr<Socket> transport = std::make_shared<Socket>(id);

	auto callback = [handler, transport, this](int fd, runtime::Mainloop::Event event) {
		if ((event & EPOLLHUP) || (event & EPOLLRDHUP)) {
			mainloop.removeEventSource(fd);
			return;
		}

		try {
			Message msg;
			msg.decode(*transport);

			CallbackHolder<void, Args...> callback(handler);
			callback.dispatch(msg);
		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	};

	mainloop.addEventSource(id, EPOLLIN | EPOLLHUP | EPOLLRDHUP, callback);

	return id;
}

template<typename Type, typename... Args>
Type Client::methodCall(const std::string& method, Args&&... args)
{
	Message request = connection->createMessage(Message::MethodCall, method);
	request.packParameters(std::forward<Args>(args)...);
	connection->send(request);

	Type response;
	Message reply = connection->dispatch();
	reply.disclose<Type>(response);

	return response;
}

} // namespace rmi
#endif //__RMI_CLIENT_H__
