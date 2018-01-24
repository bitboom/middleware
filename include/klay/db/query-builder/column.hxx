/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <tuple>

namespace qxx {

template<typename Object, typename Field>
struct Column {
	using Type = Field Object::*;
	using FieldType = Field;
	using TableType = Object;

	std::string name;
	Type type;
};

template<typename O, typename F>
Column<O, F> make_column(const std::string& name, F O::*field)
{
	return {name, field};
}

template<typename Type>
struct Distinct {
	Type value;
};

template<typename... Args>
auto distinct(Args&&... args) -> decltype(Distinct<std::tuple<Args...>>())
{
	return {std::make_tuple(std::forward<Args>(args)...)};
}

} // namespace qxx
