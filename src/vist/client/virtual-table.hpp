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

#pragma once

#include <vist/exception.hpp>

#include <map>
#include <string>
#include <vector>

namespace vist {

template <typename T>
class VirtualRow final {
public:
	using KeyValuePair = std::map<std::string, std::string>;

	explicit VirtualRow();
	explicit VirtualRow(KeyValuePair&&);

	template<typename Struct, typename Member>
	Member at(Member Struct::*) const;

	template<typename Struct, typename Member>
	Member operator[](Member Struct::*) const;

	inline std::size_t size() const { return data.size(); }

private:
	KeyValuePair data;
};

template <typename T>
class VirtualTable final {
public:
	explicit VirtualTable();

	/// Make iteratable
	using Iter = typename std::vector<VirtualRow<T>>::iterator;
	using CIter = typename std::vector<VirtualRow<T>>::const_iterator;

	inline Iter begin() { return rows.begin(); }
	inline CIter begin() const { return rows.cbegin(); }
	inline Iter end() { return rows.end(); }
	inline CIter end() const { return rows.end(); }

	inline std::size_t size() const { return rows.size(); }

	template<typename Struct, typename Member>
	VirtualRow<T>& filter(Member Struct::*field, const std::string& name)
	{
		for (auto& row : this->rows)
			if (row[field] == name)
				return row;

		THROW(ErrCode::RuntimeError) << "Not exist field: " << name;
	}

private:
	std::vector<VirtualRow<T>> rows;
};

} // namespace vist
