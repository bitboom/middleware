/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <klay/rmi/client.h>

namespace rmi {

Client::Client(const std::string& path) :
	address(path)
{
}

Client::~Client()
{
	try {
		disconnect();
	} catch (runtime::Exception& e) {}
}

void Client::connect()
{
	connection = std::make_shared<Connection>(Socket::connect(address));

	dispatcher = std::thread([this] { mainloop.run(); });
}

int Client::unsubscribe(const std::string& provider, int id)
{
	// file descriptor(id) is automatically closed when mainloop callback is destroyed.
	mainloop.removeEventSource(id);
	return 0;
}

int Client::subscribe(const std::string& provider, const std::string& name)
{
	Message request = connection->createMessage(Message::MethodCall, provider);
	request.packParameters(name);
	connection->send(request);

	runtime::FileDescriptor response;
	Message reply = connection->dispatch();
	reply.disclose(response);

	return response.fileDescriptor;
}

void Client::disconnect()
{
	mainloop.stop();
	if (dispatcher.joinable()) {
		dispatcher.join();
	}
}

} // namespace rmi
