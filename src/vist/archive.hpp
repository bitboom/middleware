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
 * @brief   Define archive interface for serializer and parameter-pack.
 * @details 1. Serializer: Serialize/deserialize below types.
 *             (fundamental types, archival object, unique_ptr, shared_ptr)
 *          2. Parameter-pack: Pack/unpack zero or more template arguments.
 */

#pragma once

#include <vist/index-sequence.hpp>

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace vist {

class Archival;

template<typename T>
using IsFundamental = typename std::enable_if<std::is_fundamental<T>::value, int>::type;

template<typename T>
using IsArchival = typename std::enable_if<std::is_base_of<Archival, T>::value, int>::type;

class Archive final {
public:
	template<typename Front, typename... Rest>
	void pack(const Front& front, const Rest& ... rest);
	inline void pack(void) {}

	template<typename Front, typename... Rest>
	void unpack(Front& front, Rest& ... rest);
	inline void unpack(void) {}

	template<typename... Ts>
	void transform(std::tuple<Ts...>& tuple);

	// serialize method
	template<typename T, IsFundamental<T> = 0>
	Archive & operator<<(const T& value);
	template<typename T, IsArchival<T> = 0>
	Archive & operator<<(const T& object);
	template<typename T>
	Archive& operator<<(const std::vector<T>& values);
	template<typename K, typename V>
	Archive& operator<<(const std::map<K, V>& map);
	template<typename T>
	Archive& operator<<(const std::unique_ptr<T>& pointer);
	template<typename T>
	Archive& operator<<(const std::shared_ptr<T>& pointer);
	Archive& operator<<(const std::string& value);
	Archive& operator<<(const Archive& archive);

	// deserialize method
	template<typename T, IsFundamental<T> = 0>
	Archive & operator>>(T& value);
	template<typename T, IsArchival<T> = 0>
	Archive & operator>>(T& object);
	template<typename T>
	Archive& operator>>(std::vector<T>& values);
	template<typename K, typename V>
	Archive& operator>>(std::map<K, V>& map);
	template<typename T>
	Archive& operator>>(std::unique_ptr<T>& pointer);
	template<typename T>
	Archive& operator>>(std::shared_ptr<T>& pointer);
	Archive& operator>>(std::string& value);
	Archive& operator>>(Archive& archive);

	unsigned char* get(void) noexcept;
	std::size_t size(void) const noexcept;
	void resize(std::size_t size);
	std::vector<unsigned char>& getBuffer(void) noexcept;

protected:
	void save(const void* bytes, std::size_t size);
	void load(void* bytes, std::size_t size);

private:
	template<typename T>
	void transformImpl(T& tuple, EmptySequence);
	template<typename T, std::size_t... I>
	void transformImpl(T& tuple, IndexSequence<I...>);

	std::vector<unsigned char> buffer;
	std::size_t current = 0;
};

class Archival {
public:
	virtual ~Archival() = default;

	virtual void pack(Archive& archive) const = 0;
	virtual void unpack(Archive& archive) = 0;
};

template<typename Front, typename... Rest>
void Archive::pack(const Front& front, const Rest& ... rest)
{
	*this << front;
	this->pack(rest...);
}

template<typename Front, typename... Rest>
void Archive::unpack(Front& front, Rest& ... rest)
{
	*this >> front;
	this->unpack(rest...);
}

template<typename... Ts>
void Archive::transform(std::tuple<Ts...>& tuple)
{
	constexpr auto size = std::tuple_size<std::tuple<Ts...>>::value;
	this->transformImpl(tuple, make_index_sequence<size>());
}

template<typename T>
void Archive::transformImpl(T&, EmptySequence)
{
}

template<typename T, std::size_t... I>
void Archive::transformImpl(T& tuple, IndexSequence<I...>)
{
	this->unpack(std::get<I>(tuple)...);
}

template<typename T, IsFundamental<T>>
Archive& Archive::operator<<(const T& value)
{
	this->save(reinterpret_cast<const void*>(&value), sizeof(value));

	return *this;
}

template<typename T>
Archive& Archive::operator<<(const std::vector<T>& values)
{
	*this << values.size();
	for (const T& value : values)
		*this << value;

	return *this;
}

template<typename K, typename V>
Archive& Archive::operator<<(const std::map<K, V>& map)
{
	*this << map.size();
	for (const auto& pair : map) {
		*this << pair.first;
		*this << pair.second;
	}

	return *this;
}

template<typename T>
Archive& Archive::operator<<(const std::unique_ptr<T>& pointer)
{
	return *this << *pointer;
}

template<typename T>
Archive& Archive::operator<<(const std::shared_ptr<T>& pointer)
{
	return *this << *pointer;
}

template<typename T, IsArchival<T>>
Archive& Archive::operator<<(const T& object)
{
	object.pack(*this);

	return *this;
}

template<typename T, IsFundamental<T>>
Archive& Archive::operator>>(T& value)
{
	this->load(reinterpret_cast<void*>(&value), sizeof(value));

	return *this;
}

template<typename T>
Archive& Archive::operator>>(std::vector<T>& values)
{
	std::size_t size;
	*this >> size;
	values.resize(size);

	for (T& value : values)
		*this >> value;

	return *this;
}

template<typename K, typename V>
Archive& Archive::operator>>(std::map<K, V>& map)
{
	std::size_t size;
	*this >> size;

	while (size--) {
		K key;
		V value;

		*this >> key;
		*this >> value;

		map[key] = value;
	}

	return *this;
}

template<typename T>
Archive& Archive::operator>>(std::unique_ptr<T>& pointer)
{
	if (pointer == nullptr)
		pointer.reset(new T);

	return *this >> *pointer;
}

template<typename T>
Archive& Archive::operator>>(std::shared_ptr<T>& pointer)
{
	if (pointer == nullptr)
		pointer.reset(new T);

	return *this >> *pointer;
}

template<typename T, IsArchival<T>>
Archive& Archive::operator>>(T& object)
{
	object.unpack(*this);

	return *this;
}

} // namespace vist
