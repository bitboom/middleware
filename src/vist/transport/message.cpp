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
/*
 * @file   message.cpp
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Implementaion of message.
 */

#include "message.hpp"

namespace vist {
namespace transport {

Message::Message(unsigned int type, const std::string& signature) :
	header({0, type, signature.size()}),
	signature(signature)
{
	this->enclose(signature);
}

Message::Message(Header header) : header(header)
{
	this->buffer.resize(this->header.length);
}

std::size_t Message::size(void) const noexcept
{
	return this->header.length;
}

void Message::resize(std::size_t size)
{
	this->buffer.resize(size);
}

std::vector<unsigned char>& Message::getBuffer(void) noexcept
{
	return this->buffer.getBuffer();
}

} // namespace transport
} // namespace vist
