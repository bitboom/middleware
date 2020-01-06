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

#include <vist/rmi/message.hpp>
#include <vist/rmi/impl/server.hpp>

#include <boost/asio.hpp>

namespace vist {
namespace rmi {
namespace impl {
namespace general {

struct Protocol {
	using Acceptor = boost::asio::local::stream_protocol::acceptor;
	using Context = boost::asio::io_service;
	using Endpoint = boost::asio::local::stream_protocol::endpoint;
	using Socket = boost::asio::local::stream_protocol::socket;

	static Message Recv(Socket& socket);
	static void Send(Socket& socket, Message& message);

	static Message Request(Socket& socket, Message& message);

	/// Passing shared_from_this() to lambda() guarantees
	/// that the lambda() always refer to a live object.
	class Async : public std::enable_shared_from_this<Async> {
	public:
		explicit Async(Context& context) : socket(context) {}
		void dispatch(const interface::Task& task);
		void process(const interface::Task& task);

		inline Socket& getSocket()
		{
			return this->socket;
		}

	private:
		Message message; 
		Socket socket;
	};
};

} // namespace general
} // namespace impl
} // namespace rmi
} // namespace vist
