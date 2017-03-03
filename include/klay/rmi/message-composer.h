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

#ifndef __RMI_MESSAGE_COMPOSER_H__
#define __RMI_MESSAGE_COMPOSER_H__

#include <cstddef>

namespace rmi {

class MessageComposer {
public:
	MessageComposer(size_t caps = 4096);
	MessageComposer(const MessageComposer& rhs);
	MessageComposer(MessageComposer&& rhs);

	~MessageComposer();

	MessageComposer& operator=(const MessageComposer& rhs);
	MessageComposer& operator=(MessageComposer&& rhs);

	void write(const void* ptr, const size_t sz);
	void read(void* ptr, const size_t sz);

	void reserve(size_t size);

	void reset()
	{
		produce = consume = 0;
	}

	char* begin() const
	{
		return buffer + consume;
	}

	char* end() const
	{
		return buffer + produce;
	}

	size_t size() const
	{
		return (end() - begin());
	}

private:
	size_t capacity;
	size_t produce;
	size_t consume;
	char *buffer;
};

} // namespae rmi
#endif //__RMI_MESSAGE_COMPOSER_H__
