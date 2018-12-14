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

#ifndef __RUNTIME_SERIALIZER_H__
#define __RUNTIME_SERIALIZER_H__

#include <vector>
#include <string>
#include <utility>

#include <klay/klay.h>
#include <klay/reflection.h>

namespace klay {

template<typename T>
struct KLAY_EXPORT SerializableArgument {
	SerializableArgument(const T& arg) :
		value(arg)
	{
	}

	const T& value;
	REFLECTABLE(value)
};

template<typename T>
struct KLAY_EXPORT DeserializableArgument {
	DeserializableArgument(T& arg) :
		value(arg)
	{
	}

	T& value;
	REFLECTABLE(value)
};

template<class StorageType>
class KLAY_EXPORT Serializer {
public:
	Serializer(StorageType& source) :
		storage(source)
	{
	}

	template<typename DataType>
	void visit(const std::string&, const DataType& value)
	{
		visitInternal(value);
	}

private:
	void visitInternal(const std::string& value)
	{
		visitInternal(value.size());
		storage.write(value.c_str(), value.size());
	}

	template<typename DataType, typename std::enable_if<std::is_arithmetic<DataType>::value, int>::type = 0>
	void visitInternal(const DataType& value)
	{
		storage.write(&value, sizeof(DataType));
	}

	template<typename DataType, typename std::enable_if<::IsReflectable<DataType>::value, int>::type = 0>
	void visitInternal(const DataType& value)
	{
		value.accept(*this);
	}

	template<typename DataType>
	void visitInternal(const std::vector<DataType>& values)
	{
		visitInternal(values.size());
		for (const DataType& value : values) {
			visitInternal(value);
		}
	}

private:
	StorageType& storage;
};

template<class StorageType>
class KLAY_EXPORT Deserializer {
public:
	Deserializer(StorageType& source) :
		storage(source)
	{
	}

	template<typename DataType>
	void visit(const std::string&, DataType& value)
	{
		visitInternal(value);
	}

private:
	void visitInternal(std::string& value)
	{
		size_t size;
		visitInternal(size);
		value.resize(size);
		storage.read(&value.front(), size);
	}

	template<typename DataType, typename std::enable_if<std::is_arithmetic<DataType>::value, int>::type = 0>
	void visitInternal(DataType& value)
	{
		storage.read(&value, sizeof(DataType));
	}

	template<typename T, typename std::enable_if<::IsReflectable<T>::value, int>::type = 0>
	void visitInternal(T& value)
	{
		value.accept(*this);
	}

	template<typename DataType>
	void visitInternal(std::vector<DataType>& values)
	{
		size_t size;
		visitInternal(size);
		values.resize(size);

		for (DataType& value : values) {
			visitInternal(value);
		}
	}

private:
	StorageType& storage;
};

} // namespace klay

namespace runtime = klay;

#endif //__RUNTIME_SERIALIZER_H__
