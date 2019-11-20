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

#include <vist/archive.hpp>

#include <algorithm>
#include <cstring>
#include <iterator>

namespace vist {

Archive& Archive::operator<<(const Archive& archive)
{
	auto data = archive.buffer;
	auto index = archive.current;
	std::copy(data.begin() + index, data.end(), std::back_inserter(this->buffer));
}

Archive& Archive::operator<<(const std::string& value)
{
	std::size_t size = value.size();
	this->save(reinterpret_cast<const void*>(&size), sizeof(size));
	this->save(reinterpret_cast<const void*>(value.c_str()), value.size());

	return *this;
}

Archive& Archive::operator>>(Archive& archive)
{
	auto data = this->buffer;
	auto index = this->current;
	std::copy(data.begin() + index, data.end(), std::back_inserter(archive.buffer));
}

Archive& Archive::operator>>(std::string& value)
{
	std::size_t size;
	this->load(reinterpret_cast<void*>(&size), sizeof(size));

	value.resize(size);
	this->load(reinterpret_cast<void*>(&value.front()), size);

	return *this;
}

unsigned char* Archive::get(void) noexcept
{
	return this->buffer.data();
}

std::size_t Archive::size(void) const noexcept
{
	return this->buffer.size();
}

void Archive::reserve(std::size_t size) noexcept
{
	this->buffer.reserve(size);
}

void Archive::save(const void* bytes, std::size_t size)
{
	auto binary = reinterpret_cast<unsigned char*>(const_cast<void*>(bytes));
	std::vector<unsigned char> next(binary, binary + size);
	std::copy(next.begin(), next.end(), std::back_inserter(this->buffer));
}

void Archive::load(void* bytes, std::size_t size)
{
	std::memcpy(bytes, reinterpret_cast<void*>(this->buffer.data() + current), size);
	current += size;
}

} // namespace vist
