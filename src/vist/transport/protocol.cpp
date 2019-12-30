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

#include "protocol.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>

namespace vist {
namespace transport {

Message Protocol::Recv(Socket& socket)
{
	DEBUG(VIST) << "Socket read event occured.";
	Message::Header header;
	const auto& headerBuffer = boost::asio::buffer(&header, sizeof(Message::Header));
	auto readen = boost::asio::read(socket, headerBuffer);
	if (readen != sizeof(Message::Header))
		THROW(ErrCode::ProtocolBroken) << "Failed to receive message header.";

	Message content(header);
	const auto& contentBuffer = boost::asio::buffer(content.getBuffer());
	readen = boost::asio::read(socket, contentBuffer);
	if (readen != content.size())
		THROW(ErrCode::ProtocolBroken) << "Failed to receive message content.";

	content.disclose(content.signature);

	return content;
}

void Protocol::Send(Socket& socket, Message& message)
{ 
	DEBUG(VIST) << "Socket write event occured.";
	const auto& headerBuffer = boost::asio::buffer(&message.header,
												   sizeof(Message::Header));
	auto written = boost::asio::write(socket, headerBuffer);
	if (written != sizeof(Message::Header))
		THROW(ErrCode::ProtocolBroken) << "Failed to send message header.";

	const auto& contentBuffer = boost::asio::buffer(message.getBuffer(), message.size());
	written = boost::asio::write(socket, contentBuffer);
	if (written != message.size())
		THROW(ErrCode::ProtocolBroken) << "Failed to send message content.";
}

Message Protocol::Request(Socket& socket, Message& message)
{
	Protocol::Send(socket, message);
	return Protocol::Recv(socket);
}

void Protocol::Async::dispatch(const Task& task, std::atomic<bool>& polling)
{
	polling = true;
	auto self = shared_from_this();
	const auto& header = boost::asio::buffer(&this->message.header,
											 sizeof(Message::Header));
	auto handler = [self, task, &polling](const auto& error, std::size_t size) {
		if (error) {
			if (error == boost::asio::error::eof) {
				DEBUG(VIST) << "Socket EoF event occured.";
				return;
			} else {
				THROW(ErrCode::RuntimeError) << error.message();
			}
		}

		if (size != sizeof(Message::Header))
			THROW(ErrCode::ProtocolBroken) << error.message();

		/// Resize message buffer to revieved header length.
		self->message.resize(self->message.size());

		const auto& contentBuffer = boost::asio::buffer(self->message.getBuffer());
		auto readen = boost::asio::read(self->socket, contentBuffer);
		if (readen != self->message.size())
			THROW(ErrCode::ProtocolBroken) << "Failed to receive message content."
				<< readen << ", " << self->message.size();

		self->message.disclose(self->message.signature);
		self->process(task, polling);
	};

	boost::asio::async_read(self->socket, header, handler);
}

void Protocol::Async::process(const Task& task, std::atomic<bool>& polling)
{
	bool raised = false;
	std::string errMsg;
	auto onError = [&raised, &errMsg](const std::string& message) {
		ERROR(VIST) << "Failed to process task: " << message;
		raised = true;
		errMsg = message;
	};

	try {
		/// Process dispatched task.
		auto result = task(message);
		this->message = result;
	} catch (const vist::Exception<ErrCode>& e) {
		onError(e.what());
	} catch (const std::exception& e) {
		onError(e.what());
	}

	if (raised)
		this->message = Message(Message::Type::Error, errMsg);

	auto self = shared_from_this();
	const auto& headerBuffer = boost::asio::buffer(&this->message.header,
												   sizeof(Message::Header));
	auto handler = [self, task, &polling](const auto& error, std::size_t size) {
		if (error || size != sizeof(Message::Header))
			THROW(ErrCode::ProtocolBroken) << "Failed to send message header: "
										   << error.message();

		const auto& contentBuffer = boost::asio::buffer(self->message.getBuffer());
		auto written = boost::asio::write(self->socket, contentBuffer);
		if (written != self->message.size())
			THROW(ErrCode::ProtocolBroken) << "Failed to send message content.";

		/// Re-dispatch for next request.
		self->dispatch(task, polling);
	};

	boost::asio::async_write(self->socket, headerBuffer, handler);
}

} // namespace transport
} // namespace vist
