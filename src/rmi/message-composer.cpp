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

#include <cstring>
#include <algorithm>

#include <klay/rmi/message-composer.h>

namespace rmi {

MessageComposer::MessageComposer(size_t caps) :
	capacity(caps),
	produce(0),
	consume(0),
	buffer(new char[caps])
{
}

MessageComposer::MessageComposer(const MessageComposer& rhs) :
	capacity(rhs.capacity),
	produce(rhs.produce),
	consume(rhs.consume),
	buffer(new char[rhs.capacity])
{
	std::copy(rhs.buffer + consume, rhs.buffer + produce, buffer + consume);
}

MessageComposer::MessageComposer(MessageComposer&& rhs) :
	capacity(0),
	produce(0),
	consume(0),
	buffer(nullptr)
{
	buffer = rhs.buffer;
	capacity = rhs.capacity;
	produce = rhs.produce;
	consume = rhs.consume;

	// Release buffer pointer from the source object so that
	// the destructor does not free the memory multiple times.

	rhs.buffer = nullptr;
	rhs.produce = 0;
	rhs.consume = 0;
}

MessageComposer::~MessageComposer()
{
	if (buffer != nullptr) {
		delete[] buffer;
	}
}

MessageComposer& MessageComposer::operator=(const MessageComposer& rhs)
{
	if (this != &rhs) {
		delete[] buffer;

		capacity = rhs.capacity;
		produce = rhs.produce;
		consume = rhs.consume;
		buffer = new char[capacity];
		std::copy(rhs.buffer + consume, rhs.buffer + produce, buffer + consume);
	}

	return *this;
}

MessageComposer& MessageComposer::operator=(MessageComposer&& rhs)
{
	if (this != &rhs) {
		// Free existing buffer
		delete[] buffer;

		// Copy the buffer pointer and its attributes from the
		// source object.
		buffer = rhs.buffer;
		produce = rhs.produce;
		consume = rhs.consume;
		capacity = rhs.capacity;

		// Release buffer pointer from the source object so that
		// the destructor does not free the memory multiple times.
		rhs.buffer = nullptr;
		rhs.produce = 0;
		rhs.consume = 0;
		rhs.capacity = 0;
	}

	return *this;
}

void MessageComposer::write(const void* ptr, const size_t sz)
{
	size_t bytes = sz;
	if ((produce + bytes) > capacity) {
		bytes = capacity - produce;
	}

	::memcpy(reinterpret_cast<char *>(buffer + produce), ptr, bytes);
	produce += bytes;
}

void MessageComposer::read(void* ptr, const size_t sz)
{
	size_t bytes = sz;
	if ((consume + bytes) > produce) {
		bytes = produce - consume;
	}

	::memcpy(ptr, reinterpret_cast<char *>(buffer) + consume, bytes);
	consume += bytes;

	// Reset cursors if there is no data
	if (consume == produce) {
		consume = produce = 0;
	}
}

} // namespae rmi
