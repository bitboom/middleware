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

#include <klay/klay.h>
#include <klay/mainloop.h>
#include <klay/rmi/message.h>
#include <klay/rmi/connection.h>
#include <klay/rmi/callback-holder.h>

namespace rmi {

template <typename ExceptionModel>
class KLAY_EXPORT RemoteAccessClient {
public:
	RemoteAccessClient(const std::string& address);
	virtual ~RemoteAccessClient();

	RemoteAccessClient(const RemoteAccessClient&) = delete;
	RemoteAccessClient& operator=(const RemoteAccessClient&) = delete;

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

template <typename ExceptionModel>
RemoteAccessClient<ExceptionModel>::RemoteAccessClient(const std::string& path) :
	address(path)
{
}

template <typename ExceptionModel>
RemoteAccessClient<ExceptionModel>::~RemoteAccessClient()
{
	try {
		disconnect();
	} catch (runtime::Exception& e) {}
}

template <typename ExceptionModel>
void RemoteAccessClient<ExceptionModel>::connect()
{
	connection = std::make_shared<Connection>(Socket::connect(address));

	dispatcher = std::thread([this] { mainloop.run(); });
}

template <typename ExceptionModel>
int RemoteAccessClient<ExceptionModel>::unsubscribe(const std::string& provider, int id)
{
	// file descriptor(id) is automatically closed when mainloop callback is destroyed.
	mainloop.removeEventSource(id);
	return 0;
}

template <typename ExceptionModel>
int RemoteAccessClient<ExceptionModel>::subscribe(const std::string& provider, const std::string& name)
{
	Message request = connection->createMessage(Message::MethodCall, provider);
	request.packParameters(name);
	connection->send(request);

	runtime::FileDescriptor response;
	Message reply = connection->dispatch();
	if (reply.isError()) {
		std::string klass;
		reply.disclose(klass);

		ExceptionModel exception;
		exception.raise(reply.target(), klass);
	}

	reply.disclose(response);

	return response.fileDescriptor;
}

template <typename ExceptionModel>
void RemoteAccessClient<ExceptionModel>::disconnect()
{
	mainloop.stop();
	if (dispatcher.joinable()) {
		dispatcher.join();
	}
}

template <typename ExceptionModel>
template <typename... Args>
int RemoteAccessClient<ExceptionModel>::subscribe(const std::string& provider, const std::string& name,
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

template <typename ExceptionModel>
template <typename Type, typename... Args>
Type RemoteAccessClient<ExceptionModel>::methodCall(const std::string& method, Args&&... args)
{
	Message request = connection->createMessage(Message::MethodCall, method);
	request.packParameters(std::forward<Args>(args)...);
	connection->send(request);

	Type response;
	Message reply = connection->dispatch();
	if (reply.isError()) {
		std::string klass;
		reply.disclose(klass);
		ExceptionModel exception;
		exception.raise(reply.target(), klass);
	}

	reply.disclose<Type>(response);

	return response;
}

class KLAY_EXPORT DefaultExceptionModel {
public:
	void raise(const std::string& target, const std::string& except);
};

using Client = RemoteAccessClient<DefaultExceptionModel>;

} // namespace rmi
#endif //__RMI_CLIENT_H__
