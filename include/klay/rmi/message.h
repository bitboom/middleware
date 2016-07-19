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

#ifndef __RMI_MESSAGE_H__
#define __RMI_MESSAGE_H__

#include <deque>
#include <string>
#include <memory>
#include <atomic>

#include <klay/serialize.h>
#include <klay/file-descriptor.h>
#include <klay/rmi/message-composer.h>

namespace rmi {

class Message {
public:
	enum Type {
		Invalid,
		MethodCall,
		Reply,
		Error,
		Signal
	};

	Message();
	Message(unsigned int id, unsigned int type, const std::string&);
	Message(unsigned int type, const std::string&);
	Message(const Message& rhs);
	Message(Message&& rhs);

	~Message();

	Message& operator=(const Message& rhs);
	Message& operator=(Message&& rhs);

	// [TBD] Take arguments
	Message createReplyMessage() const;
	Message createErrorMessage(const std::string& message) const;

	unsigned int id() const
	{
		return signature.id;
	}

	unsigned int type() const
	{
		return signature.type;
	}

	const std::string& target() const
	{
		return signature.target;
	}

	bool isInvalid() const
	{
		return type() == Invalid;
	}

	bool isError() const
	{
		return type() == Error;
	}

	bool isMethodCall() const
	{
		return type() == MethodCall;
	}

	bool isSignal() const
	{
		return type() == Signal;
	}

	bool isReply() const
	{
		return type() == Reply;
	}

	template<typename T>
	void encode(const T& device) const;

	template<typename T>
	void decode(const T& device);

	void packParameters()
	{
	}

	template<typename F>
	void packParameters(F&& arg);

	template<typename F, typename...R>
	void packParameters(F&& first, R&&... rest);

	void unpackParameters()
	{
	}

	template<typename F>
	void unpackParameters(F& arg);

	template<typename F, typename... R>
	void unpackParameters(F& first, R&... rest);

	template<typename DataType>
	void enclose(DataType&& data);

	template<typename DataType>
	void disclose(DataType& data);

private:
	struct MessageHeader {
		unsigned int id;    // Unique message id
		unsigned int type;  // Mesage type
		size_t length;// Total message length except MessageHeader itself
		size_t ancillary;
	};

	struct MessageSignature {
		unsigned int id;
		unsigned int type;
		std::string target;

		REFLECTABLE(
			id,
			type,
			target
		)
	};

	MessageSignature signature;
	MessageComposer buffer;
	std::deque<runtime::FileDescriptor> fileDescriptors;

	static std::atomic<unsigned int> sequence;
};

template<typename F>
void Message::packParameters(F&& arg)
{
	enclose<F>(std::forward<F>(arg));
}

template<typename F, typename...R>
void Message::packParameters(F&& first, R&&... rest)
{
	packParameters(std::forward<F>(first));
	packParameters(std::forward<R>(rest)...);
}

template<typename F>
void Message::unpackParameters(F& arg)
{
	disclose<F>(arg);
}

template<typename F, typename... R>
void Message::unpackParameters(F& first, R&... rest)
{
	unpackParameters(first);
	unpackParameters(rest...);
}

template<typename DataType>
void Message::enclose(DataType&& data)
{
	runtime::Serializer<MessageComposer> serializer(buffer);
	runtime::SerializableArgument<DataType> arg(std::forward<DataType>(data));
	arg.accept(serializer);
}

template<typename DataType>
void Message::disclose(DataType& data)
{
	runtime::Deserializer<MessageComposer> deserializer(buffer);
	runtime::DeserializableArgument<DataType> arg(data);
	arg.accept(deserializer);
}

template<typename T>
void Message::encode(const T& device) const
{
	MessageHeader header = {
		signature.id,
		signature.type,
		buffer.size(),
		fileDescriptors.size()
	};

	device.write(&header, sizeof(header));
	device.write(buffer.begin(), header.length);

	int i = 0, fds[fileDescriptors.size()];
	for (const runtime::FileDescriptor& fd : fileDescriptors) {
		fds[i++] = fd.fileDescriptor;
	}

	device.sendFileDescriptors(fds, fileDescriptors.size());
}

template<typename T>
void Message::decode(const T& device)
{
	MessageHeader header;
	device.read(&header, sizeof(header));
	device.read(buffer.begin(), header.length);
	buffer.reserve(header.length);

	int fds[header.ancillary];

	device.receiveFileDescriptors(fds, header.ancillary);
	for (unsigned int i = 0; i < header.ancillary; i++) {
		fileDescriptors.emplace_back(runtime::FileDescriptor(fds[i]));
	}

	disclose(signature);
}

template<> void Message::enclose(runtime::FileDescriptor&& fd);
template<> void Message::disclose(runtime::FileDescriptor& fd);

} // namespae rmi
#endif //__RMI_MESSAGE_H__
