/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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


/**
 * @file property.h
 * @brief Provides type-safe getter method
 */


#pragma once

#include <string>
#include <map>
#include <stdexcept>

namespace osquery {

/// TBD: Consider error handling.
template <typename T>
class Property {
public:
	using KeyValuePair = std::map<std::string, std::string>;

	explicit Property();
	explicit Property(KeyValuePair&&);

	template<typename Struct, typename Member>
	Member at(Member Struct::*) const;

	template<typename Struct, typename Member>
	Member operator[](Member Struct::*) const;

	inline std::size_t size() const { return data.size(); }

private:
	KeyValuePair data;
};


template <typename T>
class Properties {
public:
	explicit Properties();

	/// Make iteratable
	using Iter = typename std::vector<Property<T>>::iterator;
	using CIter = typename std::vector<Property<T>>::const_iterator;

	inline Iter begin() { return datas.begin(); }
	inline CIter begin() const { return datas.cbegin(); }
	inline Iter end() { return datas.end(); }
	inline CIter end() const { return datas.end(); }

	inline std::size_t size() const { return datas.size(); }

private:
	std::vector<Property<T>> datas;
};

} // namespace osquery
