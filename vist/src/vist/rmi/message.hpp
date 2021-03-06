/*
 *  Copyright (c) 2018-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <string>
#include <vector>

#include <vist/archive.hpp>
#include <vist/macro.hpp>

namespace vist {
namespace rmi {

struct VIST_API Message final {
	enum Type : unsigned int {
		Invalid,
		MethodCall,
		Reply,
		Error,
		Signal
	};

	struct Header {
		unsigned int id;
		unsigned int type;
		size_t length;
	};

	explicit Message(void) = default;
	explicit Message(unsigned int type, const std::string& signature);
	explicit Message(Header header);

	~Message(void) noexcept = default;

	Message(const Message&) = default;
	Message(Message&&) = default;

	Message& operator=(const Message&) = default;
	Message& operator=(Message&&) = default;

	template<typename... Args>
	void enclose(Args&& ... args);
	template<typename... Args>
	void disclose(Args& ... args);

	bool success() const noexcept;
	bool error() const noexcept;
	std::string what() const noexcept;

	std::size_t size(void) const noexcept;
	void resize(std::size_t size);
	std::vector<unsigned char>& getBuffer(void) noexcept;

	Header header;
	std::string signature;
	Archive buffer;
};

template<typename... Args>
void Message::enclose(Args&& ... args)
{
	this->buffer.pack(std::forward<Args>(args)...);
	header.length = this->buffer.size();
}

template<typename... Args>
void Message::disclose(Args& ... args)
{
	this->buffer.unpack(args...);
}

} // namespace rmi
} // namespace vist
