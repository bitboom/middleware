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
#include <klay/exception.h>
#include <klay/rmi/message.h>

namespace rmi {

std::atomic<unsigned int> Message::sequence(0);

Message::Message() :
	signature({sequence++, Invalid, ""})
{
}

Message::Message(unsigned int id, unsigned int type, const std::string& target) :
	signature({id, type, target})
{
	enclose(signature);
}

Message::Message(unsigned int type, const std::string& target) :
	signature({sequence++, type, target})
{
	enclose(signature);
}

Message::Message(Message&& rhs)
	: signature(std::move(rhs.signature)),
	  buffer(std::move(rhs.buffer)),
	  fileDescriptors(std::move(rhs.fileDescriptors))
{
}

Message::~Message()
{
}

Message::Message(const Message& rhs) :
	signature(rhs.signature),
	buffer(rhs.buffer)
{
	enclose(signature);
}

Message& Message::operator=(const Message& rhs)
{
	if (this != &rhs) {
		buffer = rhs.buffer;
		signature = rhs.signature;
	}

	return *this;
}

Message& Message::operator=(Message&& rhs)
{
	if (this != &rhs) {
		buffer = std::move(rhs.buffer);
		signature = std::move(rhs.signature);
		fileDescriptors = std::move(rhs.fileDescriptors);
	}

	return *this;
}

Message Message::createReplyMessage() const
{
	return Message(id(), Reply, target());
}

Message Message::createErrorMessage(const std::string& message) const
{
	Message error(id(), Error, target());
	error.enclose(message);

	return error;
}

template<> void Message::enclose(runtime::FileDescriptor&& fd)
{
	if (fd.fileDescriptor == -1) {
		throw runtime::Exception("Invalid file descriptor");
	}

	fileDescriptors.push_back(std::move(fd));
}

template<> void Message::disclose(runtime::FileDescriptor& fd)
{
	if (!fileDescriptors.empty()) {
		fd.fileDescriptor = std::move(fileDescriptors.front()).fileDescriptor;
		fileDescriptors.pop_front();
	}
}

} // namespace rmi
