/*
 * Copyright (c) 2011 - 2018 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file    serialization.h
 * @author  Tomasz Swierczek (t.swierczek@samsung.com)
 * @version 1.0
 * @brief   Interfaces and templates used for data serialization.
 */
#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <memory>

#include <dpl/exception.h>

namespace AuthPasswd {

class SerializationException {
public:
	DECLARE_EXCEPTION_TYPE(AuthPasswd::Exception, Base)
	DECLARE_EXCEPTION_TYPE(Base, InvalidStreamData)
};

// Abstract data stream buffer
class IStream {
public:
	virtual void Read(size_t num, void *bytes) = 0;
	virtual void Write(size_t num, const void *bytes) = 0;
	virtual ~IStream() {}
};

// Serializable interface
class ISerializable {
public:
	ISerializable() {}
	ISerializable(IStream &) {}
	virtual void Serialize(IStream &) const = 0;
	virtual ~ISerializable() {}
};

struct Serialization {
	// serialization
	// normal functions

	// ISerializable objects
	static void Serialize(IStream &stream, const ISerializable &object) {
		object.Serialize(stream);
	}
	static void Serialize(IStream &stream, const ISerializable *const object) {
		object->Serialize(stream);
	}

	// char
	static void Serialize(IStream &stream, const char value) {
		stream.Write(sizeof(value), &value);
	}
	static void Serialize(IStream &stream, const char *const value) {
		stream.Write(sizeof(*value), value);
	}

	// unsigned char
	static void Serialize(IStream &stream, const unsigned char value) {
		stream.Write(sizeof(value), &value);
	}
	static void Serialize(IStream &stream, const unsigned char *const value) {
		stream.Write(sizeof(*value), value);
	}

	// unsigned int
	static void Serialize(IStream &stream, const unsigned value) {
		stream.Write(sizeof(value), &value);
	}
	static void Serialize(IStream &stream, const unsigned *const value) {
		stream.Write(sizeof(*value), value);
	}

	// int
	static void Serialize(IStream &stream, const int value) {
		stream.Write(sizeof(value), &value);
	}
	static void Serialize(IStream &stream, const int *const value) {
		stream.Write(sizeof(*value), value);
	}

	// bool
	static void Serialize(IStream &stream, const bool value) {
		stream.Write(sizeof(value), &value);
	}
	static void Serialize(IStream &stream, const bool *const value) {
		stream.Write(sizeof(*value), value);
	}

	// time_t
	static void Serialize(IStream &stream, const time_t value) {
		stream.Write(sizeof(value), &value);
	}
	static void Serialize(IStream &stream, const time_t *const value) {
		stream.Write(sizeof(*value), value);
	}

	// std::string
	static void Serialize(IStream &stream, const std::string &str) {
		int length = str.size();
		stream.Write(sizeof(length), &length);
		stream.Write(length, str.c_str());
	}
	static void Serialize(IStream &stream, const std::string *const str) {
		int length = str->size();
		stream.Write(sizeof(length), &length);
		stream.Write(length, str->c_str());
	}

	// STL templates

	// std::list
	template <typename T>
	static void Serialize(IStream &stream, const std::list<T> &list) {
		int length = list.size();
		stream.Write(sizeof(length), &length);

		for (typename std::list<T>::const_iterator list_iter = list.begin();
				list_iter != list.end(); list_iter++)
			Serialize(stream, *list_iter);
	}
	template <typename T>
	static void Serialize(IStream &stream, const std::list<T> *const list) {
		Serialize(stream, *list);
	}

	// std::vector
	template <typename T>
	static void Serialize(IStream &stream, const std::vector<T> &vec) {
		int length = vec.size();
		stream.Write(sizeof(length), &length);

		for (typename std::vector<T>::const_iterator vec_iter = vec.begin();
				vec_iter != vec.end(); vec_iter++)
			Serialize(stream, *vec_iter);
	}
	template <typename T>
	static void Serialize(IStream &stream, const std::vector<T> *const vec) {
		Serialize(stream, *vec);
	}

	// std::set
	template <typename T>
	static void Serialize(IStream &stream, const std::set<T> &set) {
		size_t length = set.size();
		stream.Write(sizeof(length), &length);

		for (const auto &item : set)
			Serialize(stream, item);
	}

	// std::pair
	template <typename A, typename B>
	static void Serialize(IStream &stream, const std::pair<A, B> &p) {
		Serialize(stream, p.first);
		Serialize(stream, p.second);
	}
	template <typename A, typename B>
	static void Serialize(IStream &stream, const std::pair<A, B> *const p) {
		Serialize(stream, *p);
	}

	// std::map
	template <typename K, typename T>
	static void Serialize(IStream &stream, const std::map<K, T> &map) {
		int length = map.size();
		stream.Write(sizeof(length), &length);
		typename std::map<K, T>::const_iterator it;

		for (it = map.begin(); it != map.end(); ++it) {
			Serialize(stream, (*it).first);
			Serialize(stream, (*it).second);
		}
	}
	template <typename K, typename T>
	static void Serialize(IStream &stream, const std::map<K, T> *const map) {
		Serialize(stream, *map);
	}

	// std::unique_ptr
	template <typename T>
	static void Serialize(IStream &stream, const std::unique_ptr<T> &p) {
		Serialize(stream, *p);
	}

	// std::shared_ptr
	template <typename T>
	static void Serialize(IStream &stream, const std::shared_ptr<T> &p) {
		Serialize(stream, *p);
	}
}; // struct Serialization

struct Deserialization {
	// deserialization
	// normal functions

	// ISerializable objects
	// T instead of ISerializable is needed to call proper constructor
	template <typename T>
	static void Deserialize(IStream &stream, T &object) {
		object = T(stream);
	}
	template <typename T>
	static void Deserialize(IStream &stream, T *&object) {
		object = new T(stream);
	}

	// char
	static void Deserialize(IStream &stream, char &value) {
		stream.Read(sizeof(value), &value);
	}
	static void Deserialize(IStream &stream, char *&value) {
		value = new char;
		std::unique_ptr<char> ptr(value);
		stream.Read(sizeof(*value), value);
		ptr.release();
	}

	// unsigned char
	static void Deserialize(IStream &stream, unsigned char &value) {
		stream.Read(sizeof(value), &value);
	}
	static void Deserialize(IStream &stream, unsigned char *&value) {
		value = new unsigned char;
		std::unique_ptr<unsigned char> ptr(value);
		stream.Read(sizeof(*value), value);
		ptr.release();
	}

	// unsigned int
	static void Deserialize(IStream &stream, unsigned &value) {
		stream.Read(sizeof(value), &value);
	}
	static void Deserialize(IStream &stream, unsigned *&value) {
		value = new unsigned;
		std::unique_ptr<unsigned> ptr(value);
		stream.Read(sizeof(*value), value);
		ptr.release();
	}

	// int
	static void Deserialize(IStream &stream, int &value) {
		stream.Read(sizeof(value), &value);
	}
	static void Deserialize(IStream &stream, int *&value) {
		value = new int;
		std::unique_ptr<int> ptr(value);
		stream.Read(sizeof(*value), value);
		ptr.release();
	}

	// bool
	static void Deserialize(IStream &stream, bool &value) {
		stream.Read(sizeof(value), &value);
	}
	static void Deserialize(IStream &stream, bool *&value) {
		value = new bool;
		std::unique_ptr<bool> ptr(value);
		stream.Read(sizeof(*value), value);
		ptr.release();
	}

	// time_t
	static void Deserialize(IStream &stream, time_t &value) {
		stream.Read(sizeof(value), &value);
	}
	static void Deserialize(IStream &stream, time_t *&value) {
		value = new time_t;
		std::unique_ptr<time_t> ptr(value);
		stream.Read(sizeof(*value), value);
		ptr.release();
	}

	// std::string
	static void Deserialize(IStream &stream, std::string &str) {
		int length;
		stream.Read(sizeof(length), &length);
		if (length < 0)
			ThrowMsg(SerializationException::InvalidStreamData, "Invalid length of std::string (less than 0)");
		char * buf = new char[length + 1];
		std::unique_ptr<char[]> ptr(buf);
		stream.Read(length, buf);
		buf[length] = 0;
		str = std::string(buf);
	}
	static void Deserialize(IStream &stream, std::string *&str) {
		int length;
		stream.Read(sizeof(length), &length);
		if (length < 0)
			ThrowMsg(SerializationException::InvalidStreamData, "Invalid length of std::string (less than 0)");
		char * buf = new char[length + 1];
		std::unique_ptr<char[]> ptr(buf);
		stream.Read(length, buf);
		buf[length] = 0;
		str = new std::string(buf);
	}

	// STL templates

	// std::list
	template <typename T>
	static void Deserialize(IStream &stream, std::list<T> &list) {
		int length;
		stream.Read(sizeof(length), &length);
		if (length < 0)
			ThrowMsg(SerializationException::InvalidStreamData, "Invalid length of std::list (less than 0)");
		for (int i = 0; i < length; ++i) {
			T obj;
			Deserialize(stream, obj);
			list.push_back(std::move(obj));
		}
	}
	template <typename T>
	static void Deserialize(IStream &stream, std::list<T> *&list) {
		list = new std::list<T>;
		std::unique_ptr<std::list<T>> ptr(list);
		Deserialize(stream, *list);
		ptr.release();
	}

	// std::vector
	template <typename T>
	static void Deserialize(IStream &stream, std::vector<T> &vec) {
		int length;
		stream.Read(sizeof(length), &length);
		if (length < 0)
			ThrowMsg(SerializationException::InvalidStreamData, "Invalid length of std::vector (less than 0)");
		for (int i = 0; i < length; ++i) {
			T obj;
			Deserialize(stream, obj);
			vec.push_back(std::move(obj));
		}
	}
	template <typename T>
	static void Deserialize(IStream &stream, std::vector<T> *&vec) {
		vec = new std::vector<T>;
		std::unique_ptr<std::vector<T>> ptr(vec);
		Deserialize(stream, *vec);
		ptr.release();
	}

	// std::set
	template <typename T>
	static void Deserialize(IStream &stream, std::set<T> &set) {
		size_t length;
		stream.Read(sizeof(length), &length);
		for (size_t i = 0; i < length; ++i) {
			T obj;
			Deserialize(stream, obj);
			set.insert(std::move(obj));
		}
	}

	// std::pair
	template <typename A, typename B>
	static void Deserialize(IStream &stream, std::pair<A, B> &p) {
		Deserialize(stream, p.first);
		Deserialize(stream, p.second);
	}
	template <typename A, typename B>
	static void Deserialize(IStream &stream, std::pair<A, B> *&p) {
		p = new std::pair<A, B>;
		std::unique_ptr<std::pair<A, B>> ptr(p);
		Deserialize(stream, *p);
		ptr.release();
	}

	// std::tuple
	template <std::size_t I = 0, typename... Tp>
	static inline typename std::enable_if<I == sizeof...(Tp), void>::type
	Deserialize(IStream&, std::tuple<Tp...>&)
	{}

	template <std::size_t I = 0, typename... Tp>
	static inline typename std::enable_if<I < sizeof...(Tp), void>::type
	Deserialize(IStream& stream, std::tuple<Tp...>& t) {
		Deserialize(stream, std::get<I>(t));
		Deserialize<I+1>(stream, t);
	}

	template <typename... Tp>
	static void Deserialize(IStream &stream, std::tuple<Tp...> *&t) {
		t = new std::tuple<Tp...>;
		std::unique_ptr<std::tuple<Tp...>> ptr(t);
		Deserialize(stream, *t);
		ptr.release();
	}

	// std::map
	template <typename K, typename T>
	static void Deserialize(IStream &stream, std::map<K, T> &map) {
		int length;
		stream.Read(sizeof(length), &length);
		if (length < 0)
			ThrowMsg(SerializationException::InvalidStreamData, "Invalid size of std::map (less than 0)");
		for (int i = 0; i < length; ++i) {
			K key;
			T obj;
			Deserialize(stream, key);
			Deserialize(stream, obj);
			map[key] = std::move(obj);
		}
	}
	template <typename K, typename T>
	static void Deserialize(IStream &stream, std::map<K, T> *&map) {
		map = new std::map<K, T>;
		std::unique_ptr<std::map<K, T>> ptr(map);
		Deserialize(stream, *map);
		ptr.release();
	}

	template<typename T1, typename T2, typename... Tail>
	static void Deserialize(IStream &stream, T1 &first, T2 &second, Tail&... tail) {
		Deserialization::Deserialize(stream, first);
		Deserialization::Deserialize(stream, second, tail...);
	}
}; // struct Deserialization
} // namespace AuthPasswd

#endif // SERIALIZATION_H
